#include "MtMethod.h"

#include "../CoreLib/Log.h"
#include "MtModLibrary.h"
#include "MtModule.h"

void log_error(MtNode n, const char* fmt, ...);

//-----------------------------------------------------------------------------
// Given two blocks that execute as branches, merge their dependencies and
// writes together.

bool merge_branch(MtDelta& a, MtDelta& b, MtDelta& out) {
  out.wipe();

  // Delta A's input and delta B's input must agree about the state of all
  // matching fields.

  for (auto& s1 : a.state_old) {
    if (b.state_old.contains(s1.first)) {
      auto& s2 = *b.state_old.find(s1.first);
      assert(s1.second != ERROR);
      assert(s2.second != ERROR);
      if (s1.second != s2.second) return false;
    }
  }

  // The merged state inputs is the union of the two delta's inputs.

  for (auto& s1 : a.state_old) out.state_old[s1.first] = s1.second;
  for (auto& s2 : b.state_old) out.state_old[s2.first] = s2.second;

  // If the two deltas disagree on the state of an output, we set it to "maybe".

  for (auto& s : a.state_new) out.state_new[s.first] = ERROR;
  for (auto& s : b.state_new) out.state_new[s.first] = ERROR;

  for (auto& s : out.state_new) {
    auto& an = a.state_new[s.first];
    auto& bn = b.state_new[s.first];

    s.second = an == bn ? an : MAYBE;
  }

  return true;
}

//------------------------------------------------------------------------------
// Given two blocks that execute in series, merge their dependencies and
// writes together.

bool merge_series(MtDelta& a, MtDelta& b, MtDelta& out) {
  // Delta A's output must be compatible with delta B's input.

  for (auto& s2 : b.state_old) {
    if (a.state_new.contains(s2.first)) {
      auto& s1 = *a.state_new.find(s2.first);
      assert(s1.second != ERROR);
      assert(s2.second != ERROR);
      if (s1.second != s2.second) {
        log_error(
            MtNode::null,
            "merge_series error - a.state_new %s = %s, b.state_old %s = %s\n",
            s1.first.c_str(), to_string(s1.second), s2.first.c_str(),
            to_string(s2.second));
        out.error = true;
        return false;
      }
    }
  }

  // The resulting state is just delta B applied on top of delta A.

  out.state_old = a.state_old;
  out.state_new = a.state_new;

  for (auto& s2 : b.state_old) out.state_old[s2.first] = s2.second;
  for (auto& s2 : b.state_new) out.state_new[s2.first] = s2.second;

  return true;
}

//------------------------------------------------------------------------------

MtMethod::MtMethod(MtNode n, MtModule* _mod, MtModLibrary* _lib)
    : MtNode(n), mod(_mod), lib(_lib) {
  assert(mod);
  assert(lib);
}

void MtMethod::update_delta() {
  if (delta == nullptr) {
    auto temp_delta = new MtDelta();
    auto body = get_field(field_body);
    check_dirty_dispatch(body, *temp_delta);
    delta = temp_delta;
  }
}

//------------------------------------------------------------------------------

void MtMethod::check_dirty_dispatch(MtNode n, MtDelta& d) {
  for (auto& n : d.state_old) assert(n.second != ERROR);
  for (auto& n : d.state_new) assert(n.second != ERROR);

  if (n.is_null() || !n.is_named()) return;

  switch (n.sym) {
    case sym_field_expression:
      check_dirty_read_submod(n, d);
      break;
    case sym_identifier:
      check_dirty_read_identifier(n, d);
      break;
    case sym_assignment_expression:
      check_dirty_assign(n, d);
      break;
    case sym_if_statement:
      check_dirty_if(n, d);
      break;
    case sym_call_expression: {
      if (n.get_field(field_function).sym == sym_field_expression) {
        // submod.tick()/tock()
        check_dirty_call(n, d);
      } else if (n.get_field(field_function).sym == sym_template_function) {
        // foo = bx<x>(bar);
        check_dirty_call(n, d);
      } else {
        // cat() etc
        check_dirty_call(n, d);
      }
      break;
    }
    case sym_switch_statement:
      check_dirty_switch(n, d);
      break;

    default: {
      for (auto c : n) check_dirty_dispatch(c, d);
    }
  }
}

//------------------------------------------------------------------------------

void MtMethod::check_dirty_read_identifier(MtNode n, MtDelta& d) {
  assert(n.sym == sym_identifier);
  auto field = n.text();

  // Only check reads of regs and outputs.
  if (!mod->has_register(field) && !mod->has_output(field)) return;

  // Reading from a dirty field in tick() is forbidden.
  if (is_tick) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] == MAYBE) {
        log_error(n, "%s() read maybe new field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      } else if (d.state_new[field] == DIRTY) {
        log_error(n, "%s() read dirty new field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] == MAYBE) {
        log_error(n, "%s() read maybe old field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      } else if (d.state_old[field] == DIRTY) {
        log_error(n, "%s() read dirty old field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else {
      // Haven't seen this field before. We're in tock(), so we expect it to be
      // clean.
      d.state_old[field] = CLEAN;
      d.state_new[field] = CLEAN;
    }
  }

  // Reading from a clean field in tock() is forbidden.
  if (is_tock) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] == CLEAN) {
        log_error(n, "%s() read clean new field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      } else if (d.state_new[field] == MAYBE) {
        log_error(n, "%s() read maybe new field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] == CLEAN) {
        log_error(n, "%s() read clean old field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      } else if (d.state_old[field] == MAYBE) {
        log_error(n, "%s() read maybe old field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else {
      // Haven't seen this field before. We're in tock(), so we expect it to be
      // dirty.
      d.state_old[field] = DIRTY;
      d.state_new[field] = DIRTY;
    }
  }
}

//------------------------------------------------------------------------------

void MtMethod::check_dirty_read_submod(MtNode n, MtDelta& d) {
  assert(n.sym == sym_field_expression);
  auto field = n.text();

  auto dot_pos = field.find('.');
  std::string submod_name = field.substr(0, dot_pos);
  std::string submod_field = field.substr(dot_pos + 1, field.size());
  auto submod_node = mod->get_submod(submod_name);
  auto submod_mod = submod_node->mod;

  if (!submod_mod->has_output(submod_field)) {
    log_error(n, "%s() read non-output from submodule - %s\n", name.c_str(),
              field.c_str());
    d.error = true;
  }

  //----------

  // Reading from a dirty field in tick() is forbidden.
  if (is_tick) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] != CLEAN) {
        log_error(n, "%s() read dirty new submod field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] != CLEAN) {
        log_error(n, "%s() read dirty old submod field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else {
      d.state_old[field] = CLEAN;
      d.state_new[field] = CLEAN;
    }
  }

  // Reading from a clean field in tock() is forbidden.
  if (is_tock) {
    if (d.state_new.contains(field)) {
      if (d.state_new[field] == CLEAN) {
        log_error(n, "%s() read clean new submod field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else if (d.state_old.contains(field)) {
      if (d.state_old[field] == CLEAN) {
        log_error(n, "%s() read clean old submod field - %s\n", name.c_str(),
                  field.c_str());
        d.error = true;
      }
    } else {
      d.state_old[field] = DIRTY;
      d.state_new[field] = DIRTY;
    }
  }
}

//------------------------------------------------------------------------------

void MtMethod::check_dirty_write(MtNode n, MtDelta& d) {
  // If the LHS is a subscript expression, check the source field.
  if (n.sym == sym_subscript_expression) {
    return check_dirty_write(n.get_field(field_argument), d);
  }

  // If the LHS is a slice expression, check the source field.
  if (n.sym == sym_call_expression) {
    auto func_name = n.get_field(field_function).text();
    for (int i = 0; i < func_name.size(); i++) {
      assert(i ? isdigit(func_name[i]) : func_name[i] == 's');
    }
    auto field_node = n.get_field(field_arguments).named_child(0);
    return check_dirty_write(field_node, d);
  }

  //----------

  assert(n.sym == sym_identifier);
  auto field = n.text();

  // Writing to inputs is forbidden.
  assert(!mod->has_input(field));

  // Only check writes to regs and outputs

  bool field_is_register = mod->has_register(field);
  bool field_is_output = mod->has_output(field);

  if (!field_is_register && !field_is_output) return;

  //----------

  if (is_tick && field_is_output) {
    log_error(n, "%s() wrote output - %s\n", name.c_str(), field.c_str());
    d.error = true;
  }

  if (is_tock && field_is_register) {
    log_error(n, "%s() wrote reg - %s\n", name.c_str(), field.c_str());
    d.error = true;
  }

  if (d.state_old.contains(field)) {
    if (d.state_old[field] != CLEAN) {
      log_error(n, "%s() wrote dirty old field - %s\n", name.c_str(),
                field.c_str());
      d.error = true;
    }
  }

  if (d.state_new.contains(field)) {
    if (d.state_new[field] != CLEAN) {
      log_error(n, "%s() wrote dirty new field - %s\n", name.c_str(),
                field.c_str());
      d.error = true;
    }
  }

  d.state_old[field] = CLEAN;
  d.state_new[field] = DIRTY;
}

//------------------------------------------------------------------------------
// Check for reads on the RHS of an assignment, then check the write on the
// left.

void MtMethod::check_dirty_assign(MtNode n, MtDelta& d) {
  auto lhs = n.get_field(field_left);
  auto rhs = n.get_field(field_right);

  check_dirty_dispatch(rhs, d);
  check_dirty_write(lhs, d);
}

//------------------------------------------------------------------------------
// Check the "if" branch and the "else" branch independently and then merge the
// results.

void MtMethod::check_dirty_if(MtNode n, MtDelta& d) {
  check_dirty_dispatch(n.get_field(field_condition), d);

  MtDelta if_delta = d;
  MtDelta else_delta = d;

  check_dirty_dispatch(n.get_field(field_consequence), if_delta);
  check_dirty_dispatch(n.get_field(field_alternative), else_delta);

  merge_branch(if_delta, else_delta, d);
}

//------------------------------------------------------------------------------
// Traverse function calls.

void MtMethod::check_dirty_call(MtNode n, MtDelta& d) {
  auto call = mod->node_to_call(n);

  auto node_args = call.get_field(field_arguments);
  assert(node_args.sym == sym_argument_list);
  check_dirty_dispatch(node_args, d);

  auto node_func = call.get_field(field_function);

  if (node_func.sym == sym_identifier) {
    // local function call, traverse args and then function body
    // TODO - traverse function body
    // printf("%s\n", node_func.text().c_str());
    // n.dump_tree();
    // debugbreak();

    // We hit this in b9(x) calls and whatnot, we need to sort them out and
    // resolve the local task/function calls.

  } else if (node_func.sym == sym_field_expression) {
    assert(call.method);
    call.method->update_delta();

    MtDelta temp_delta = d;
    MtDelta call_delta = *call.method->delta;
    call_delta.add_prefix(call.submod->name());

    merge_series(temp_delta, call_delta, d);
  } else if (node_func.sym == sym_template_function) {
    // local function call, probably bx<n>(things);
    // n.dump_tree();
    // debugbreak();
  } else {
    n.dump_tree();
    debugbreak();
  }
}

//------------------------------------------------------------------------------
// Check the condition of a switch statement, then check each case
// independently.

void MtMethod::check_dirty_switch(MtNode n, MtDelta& d) {
  check_dirty_dispatch(n.get_field(field_condition), d);

  MtDelta init_delta = d;
  MtMethod old_method = *this;

  bool first_branch = true;

  auto body = n.get_field(field_body);
  for (auto c : body) {
    if (c.sym == sym_case_statement) {
      // c.dump_tree();
      MtDelta case_delta = init_delta;
      check_dirty_dispatch(c, case_delta);

      if (first_branch) {
        d = case_delta;
        first_branch = false;
      } else {
        MtDelta temp = d;
        merge_branch(temp, case_delta, d);
      }
    }
  }
}

//------------------------------------------------------------------------------
