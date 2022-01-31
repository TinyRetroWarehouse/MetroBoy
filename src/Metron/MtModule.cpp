#include "MtModule.h"
#include <assert.h>

#include "MtIterator.h"
#include "Platform.h"
#include "../Plait/TreeSymbols.h"

#include <assert.h>

#pragma warning(disable:4996) // unsafe fopen()

extern "C" {
  extern const TSLanguage* tree_sitter_cpp();
}

//------------------------------------------------------------------------------

blob load_blob(const char* filename) {
  FILE* f = fopen(filename, "rb");
  assert(f);

  blob result;
  fseek(f, 0, SEEK_END);
  result.resize(ftell(f));
  fseek(f, 0, SEEK_SET);

  auto res = fread(result.data(), 1, result.size(), f);
  fclose(f);
  return result;
}

void print_escaped(char s) {
  if (s == '\n') printf("\\n");
  else if (s == '\r') printf("\\r");
  else if (s == '\t') printf("\\t");
  else if (s == '"')  printf("\\\"");
  else if (s == '\\') printf("\\\\");
  else                printf("%c", s);
}

void print_escaped(const char* source, uint32_t a, uint32_t b) {
  printf("\"");
  for (; a < b; a++) {
    print_escaped(source[a]);
  }
  printf("\"");
}

//------------------------------------------------------------------------------

MtModule::MtModule() {
}

MtModule::~MtModule() {

  ts_tree_delete(tree);
  ts_parser_delete(parser);

  src_blob.clear();
  lang = nullptr;
  parser = nullptr;
  tree = nullptr;
  source = nullptr;
}

void MtModule::load(const std::string& input_filename, const std::string& output_filename) {
  printf("loading %s\n", input_filename.c_str());
  this->input_filename = input_filename;
  this->output_filename = output_filename;

  parser = ts_parser_new();
  lang = tree_sitter_cpp();
  ts_parser_set_language(parser, lang);

  src_blob = load_blob(input_filename.c_str());

  source = (const char*)src_blob.data();
  source_end = source + src_blob.size();
  tree = ts_parser_parse_string(parser, NULL, source, (uint32_t)src_blob.size());
  root = ts_tree_root_node(tree);

  find_module();
  collect_moduleparams();
  collect_fields();
}

//------------------------------------------------------------------------------

TSNode MtModule::get_field_by_id(TSNode id) {
  assert(ts_node_symbol(id) == sym_identifier);
  auto name_a = node_to_name(id);

  for (auto& field : fields) {
    auto name_b = node_to_name(field);
    if (name_a == name_b) return field;
  }

  return { 0 };
}

TSNode MtModule::get_task_by_id(TSNode id) {
  assert(ts_node_symbol(id) == sym_identifier);
  auto name_a = node_to_name(id);

  for (auto& task : tasks) {
    auto name_b = node_to_name(task);
    if (name_a == name_b) return task;
  }

  return { 0 };
}

TSNode MtModule::get_function_by_id(TSNode id) {
  assert(ts_node_symbol(id) == sym_identifier);
  auto name_a = node_to_name(id);

  for (auto& func : functions) {
    auto name_b = node_to_name(func);
    if (name_a == name_b) return func;
  }

  return { 0 };
}

//------------------------------------------------------------------------------
// Node debugging

void MtModule::dump_node(TSNode n, int index, int field, int depth) {
  if (ts_node_is_null(n)) {
    printf("### NULL ###\n");
    return;
  }

  auto s = ts_node_symbol(n);

  uint32_t color = 0x00000000;
  if (s == sym_template_declaration) color = 0xAADDFF;
  if (s == sym_struct_specifier)     color = 0xFFAAFF;
  if (s == sym_class_specifier)      color = 0xFFAAFF;
  if (s == sym_expression_statement) color = 0xAAFFFF;
  if (s == sym_expression_statement) color = 0xAAFFFF;
  if (s == sym_compound_statement)   color = 0xFFFFFF;
  if (s == sym_function_definition)  color = 0xAAAAFF;
  if (s == sym_field_declaration)    color = 0xFFAAAA;
  if (s == sym_comment)              color = 0xAAFFAA;

  if (color) {
    printf("\u001b[38;2;%d;%d;%dm", (color >> 0) & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF);
    for (int i = 0; i < depth; i++) printf("|---");
  }
  else {
    for (int i = 0; i < depth; i++) printf("|   ");
  }

  printf("[%d] ", index);

  if (field != -1) printf("f%d ", field);
  if (s != -1) printf("s%d ", s);

  if (field != -1) {
    printf("%s.", ts_language_field_name_for_id(lang, field));
  }

  if (!ts_node_is_null(n) && ts_node_is_named(n) && ts_node_child_count(n)) {

    printf("%s: ", ts_node_type(n));
  }
  else if (!ts_node_is_null(n) && ts_node_is_named(n) && !ts_node_child_count(n)) {
    printf("%s: ", ts_node_type(n));
    ::print_escaped(source, ts_node_start_byte(n), ts_node_end_byte(n));
  }
  else if (ts_node_is_null(n)) {
    debugbreak();
    printf("text: ");
    ::print_escaped(source, ts_node_start_byte(n), ts_node_end_byte(n));
  }
  else {
    // Unnamed nodes usually have their node body as their "type",
    // and their symbol is something like "aux_sym_preproc_include_token1"
    printf("lit: ");
    ::print_escaped(source, ts_node_start_byte(n), ts_node_end_byte(n));
  }

  printf("\n");
  printf("\u001b[0m");
}

//------------------------------------------------------------------------------

void MtModule::dump_tree(TSNode n, int index, int field, int depth, int maxdepth) {
  if (depth == 0) {
    printf("\n========== tree dump begin\n");
  }
  dump_node(n, index, field, depth);

  if (depth < maxdepth) {
    if (!ts_node_is_null(n)) {
      for (auto c : n) {
        dump_tree(c, c.index, c.field, depth + 1, maxdepth);
      }
    }
  }
  if (depth == 0) printf("========== tree dump end\n");
}

//------------------------------------------------------------------------------
// Node traversal

void MtModule::visit_tree(TSNode n, NodeVisitor cv) {
  cv(n);
  for (auto c : n) visit_tree(c, cv);
}

void MtModule::visit_tree2(TSNode n, NodeVisitor2 cv) {
  for (auto c : n) {
    cv(n, c);
    visit_tree2(c, cv);
  }
}

//------------------------------------------------------------------------------
// Strip leading/trailing whitespace off non-SYM_LF nodes.

const char* MtModule::start(TSNode n) {
  assert(!ts_node_is_null(n));

  auto a = &source[ts_node_start_byte(n)];
  auto b = &source[ts_node_end_byte(n)];

  if (ts_node_symbol(n) == anon_sym_LF) return a;

  while (a < b && isspace(a[0])) a++;
  return a;
}

const char* MtModule::end(TSNode n) {
  assert(!ts_node_is_null(n));

  auto a = &source[ts_node_start_byte(n)];
  auto b = &source[ts_node_end_byte(n)];

  if (ts_node_symbol(n) == anon_sym_LF) return b;

  while (b > a && isspace(b[-1])) b--;
  return b;
}

//------------------------------------------------------------------------------

std::string MtModule::body(TSNode n) {
  return std::string(start(n), end(n));
}

bool MtModule::match(TSNode n, const char* s) {
  assert(!ts_node_is_null(n));

  const char* a = start(n);
  const char* b = end(n);

  while (a != b) {
    if (*a++ != *s++)  return false;
  }
  return true;
}

std::string MtModule::node_to_name(TSNode n) {
  auto sym = ts_node_symbol(n);
  switch (sym) {
  
  case sym_field_expression:
  case alias_sym_type_identifier:
  case sym_identifier:
  case alias_sym_field_identifier:
    return body(n);

  case sym_array_declarator:
  case sym_parameter_declaration:
  case sym_field_declaration:
  case sym_optional_parameter_declaration:
  case sym_function_definition:
  case sym_function_declarator:
    return node_to_name(ts_node_child_by_field_id(n, field_declarator));

  case sym_struct_specifier:
  case sym_class_specifier:
    return node_to_name(ts_node_child_by_field_id(n, field_name));

  default:
    dump_tree(n);
    debugbreak();
    return "";
  }
}

std::string MtModule::node_to_type(TSNode n) {
  auto sym = ts_node_symbol(n);
  switch (sym) {
  case alias_sym_type_identifier:
    return body(n);

  case sym_field_declaration:
    return node_to_type(ts_node_child_by_field_id(n, field_type));

  case sym_template_type:
    return node_to_type(ts_node_child_by_field_id(n, field_name));

  default:
    dump_tree(n);
    debugbreak();
    return "";
  }
}

//------------------------------------------------------------------------------
// Field introspection

bool MtModule::field_is_primitive(TSNode n) {

  auto node_type = ts_node_child_by_field_id(n, field_type);
  auto node_decl = ts_node_child_by_field_id(n, field_declarator);

  // Primitive types are primitive types.
  if (ts_node_symbol(node_type) == sym_primitive_type) return true;

  // Logic arrays are primitive types.
  if (ts_node_symbol(node_type) == sym_template_type) {
    auto templ_name = ts_node_child_by_field_id(node_type, field_name);
    if (match(templ_name, "logic")) return true;
  }

  // Bits are primitive types.
  if (match(node_type, "bit")) return true;

  return false;
}

bool MtModule::field_is_module(TSNode n) {
  return !field_is_primitive(n);
}

bool MtModule::field_is_static(TSNode n) {
  for (auto c : n) {
    if (c.sym == sym_storage_class_specifier) {
      if (match(c, "static")) return true;
    }
  }
  return false;
}

bool MtModule::field_is_const(TSNode n) {
  for (auto c : n) {
    if (c.sym == sym_type_qualifier) {
      if (match(c, "const")) return true;
    }
  }
  return false;
}

bool MtModule::field_is_param(TSNode n) {
  return field_is_static(n) && field_is_const(n);
}

bool MtModule::field_is_input(TSNode n) {
  if (field_is_static(n) || field_is_const(n)) return false;

  auto name = ts_node_child_by_field_id(n, field_declarator);
  return body(name).starts_with("i_");
}

bool MtModule::field_is_output(TSNode n) {
  if (field_is_static(n) || field_is_const(n)) return false;

  auto name = ts_node_child_by_field_id(n, field_declarator);
  return body(name).starts_with("o_");
}

//------------------------------------------------------------------------------
// Scanner

void MtModule::find_module() {
  module_template = { 0 };
  module_class = { 0 };

  visit_tree2(root, [&](TSNode parent, TSNode child) {
    auto sp = ts_node_symbol(parent);
    auto sc = ts_node_symbol(child);

    if (sc == sym_struct_specifier || sc == sym_class_specifier) {
      if (sp == sym_template_declaration) module_template = parent;
      module_class = child;

      //dump_tree(module_class, 0, -1, 0, 2);
      auto name_node = ts_node_child_by_field_id(module_class, field_name);
      module_name = body(name_node);
    }
    });

}

void MtModule::collect_moduleparams() {
  if (ts_node_is_null(module_template)) return;

  if (ts_node_symbol(module_template) != sym_template_declaration) debugbreak();

  auto params = ts_node_child_by_field_id(module_template, field_parameters);

  for (auto child : params) {
    if (child.sym == sym_parameter_declaration ||
        child.sym == sym_optional_parameter_declaration) {
      moduleparams.push_back(child);
    }
  }
}


void MtModule::collect_fields() {
  visit_tree(module_class, [&](TSNode n) {
    if (ts_node_symbol(n) == sym_function_definition) {
      //dump_tree(n, 2);
      auto func_name = ts_node_child_by_field_id(ts_node_child_by_field_id(n, field_declarator), field_declarator);
      auto func_args = ts_node_child_by_field_id(ts_node_child_by_field_id(n, field_declarator), field_parameters);

      if (match(func_name, "tick")) {
        visit_tree(func_args, [&](TSNode func_arg) {
          if (ts_node_symbol(func_arg) == sym_parameter_declaration) {
            auto arg_name = ts_node_child_by_field_id(func_arg, field_declarator);

            if (!match(arg_name, "rst_n")) {
              inputs.push_back(func_arg);
              //dump_tree(func_arg, 2);
            }
          }
        });
      }
    }
  });

  visit_tree(module_class, [&](TSNode n) {
    if (ts_node_symbol(n) == sym_field_declaration) {
      if      (field_is_input(n))  inputs.push_back(n);
      else if (field_is_output(n)) outputs.push_back(n);
      else if (field_is_param(n))  localparams.push_back(n);
      else if (field_is_module(n)) submodules.push_back(n);
      else                         fields.push_back(n);
    }
    if (ts_node_symbol(n) == sym_function_definition) {
      auto func_def = n;

      assert(ts_node_child_count(func_def) == 3);
      assert(ts_node_field_id_for_child(func_def, 0) == field_type);
      assert(ts_node_field_id_for_child(func_def, 1) == field_declarator);
      assert(ts_node_field_id_for_child(func_def, 2) == field_body);

      auto func_type = ts_node_child_by_field_id(func_def, field_type);
      auto func_decl = ts_node_child_by_field_id(func_def, field_declarator);

      bool is_task = false;
      bool is_init = false;
      bool is_tock = false;
      bool is_tick = false;

      //----------

      is_task = match(func_type, "void");

      //----------

      auto current_function_name = ts_node_child_by_field_id(func_decl, field_declarator);
      is_init = is_task && match(current_function_name, "init");
      is_tick = is_task && match(current_function_name, "tick");
      is_tock = is_task && match(current_function_name, "tock");

      if (is_init) {
        node_init = func_def;
      }
      else if (is_tick) {
        node_tick = func_def;
      }
      else if (is_tock) {
        node_tock = func_def;
      }
      else {
        if (is_task) {
          tasks.push_back(func_def);
        }
        else {
          functions.push_back(func_def);
        }
      }
    }
  });
}

//------------------------------------------------------------------------------
