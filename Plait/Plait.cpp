#include "Plait/Plait.h"

#include <fstream>

#include "json/single_include/nlohmann/json.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void from_json(const nlohmann::json& j, PlaitLabel*& plait_label) {
  plait_label = new PlaitLabel();
  plait_label->text      = j["text"];
  plait_label->pos_old.x = j["pos_x"];
  plait_label->pos_old.y = j["pos_y"];
  plait_label->scale     = j["scale"];
  plait_label->pos_new = plait_label->pos_old;
}

void to_json(nlohmann::json& j, const PlaitLabel* plait_label) {
  j["text"]  = plait_label->text;
  j["pos_x"] = plait_label->pos_old.x;
  j["pos_y"] = plait_label->pos_old.y;
  j["scale"] = plait_label->scale;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void from_json(const nlohmann::json& j, PlaitTrace*& plait_trace) {
  plait_trace = new PlaitTrace();
  plait_trace->input_cell_name  = j["input_cell"];
  plait_trace->input_node_name  = j["input_node"];
  plait_trace->output_cell_name = j["output_cell"];
  plait_trace->output_node_name = j["output_node"];
}

void to_json(nlohmann::json& j, const PlaitTrace* plait_trace) {
  j["input_cell"]  = plait_trace->input_node->plait_cell->die_cell->tag;
  j["input_node"]  = plait_trace->input_node->name;
  j["output_cell"] = plait_trace->output_node->plait_cell->die_cell->tag;
  j["output_node"] = plait_trace->output_node->name;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void from_json(const nlohmann::json& j, PlaitNode*& plait_node) {
  plait_node = new PlaitNode();

  plait_node->name      = j.value("name", "<missing_node_name>");
  plait_node->pos_old.x = j.value("pos_x", 0.0);
  plait_node->pos_old.y = j.value("pos_y", 0.0);
  plait_node->pos_new = plait_node->pos_old;
}

void to_json(nlohmann::json& j, const PlaitNode* plait_node) {
  j["name"]  = plait_node->name;
  j["pos_x"] = plait_node->pos_new.x;
  j["pos_y"] = plait_node->pos_new.y;
}

void PlaitNode::dump(Dumper& d) {
  using namespace nlohmann;
  json j = this;
  d(j.dump(2).c_str());
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void from_json(const nlohmann::json& j, PlaitCell*& plait_cell) {
  plait_cell = new PlaitCell();
  j["core"].get_to(plait_cell->core_node);
  if (j.contains("roots")) j["roots"].get_to(plait_cell->root_nodes);
  j["leaves"].get_to(plait_cell->leaf_nodes);
}

//----------------------------------------

void to_json(nlohmann::json& j, const PlaitCell* plait_cell) {
  j["core"] = plait_cell->core_node;
  j["roots"] = plait_cell->root_nodes;
  j["leaves"] = plait_cell->leaf_nodes;
}

//--------------------------------------------------------------------------------

PlaitCell::~PlaitCell() {
  delete core_node;
  for (auto& [name, leaf] : leaf_nodes) {
    delete leaf;
  }
}

//--------------------------------------------------------------------------------

void PlaitCell::dump(Dumper& d) {
  using namespace nlohmann;
  json j = this;
  d("Cell %s:\n", die_cell->tag.c_str());
  d(j.dump(2).c_str());
  d("\n");
}

//--------------------------------------------------------------------------------

void PlaitCell::add_root_node(PlaitNode* node) {
  CHECK_N(node->plait_cell);
  CHECK_N(node->name == "core");
  CHECK_N(root_nodes[node->name]);

  root_nodes[node->name] = node;
  node->plait_cell = this;
}

void PlaitCell::add_leaf_node(PlaitNode* node) {
  CHECK_N(node->plait_cell);
  CHECK_N(node->name == "core");
  CHECK_N(leaf_nodes[node->name]);

  leaf_nodes[node->name] = node;
  node->plait_cell = this;
}

//--------------------------------------------------------------------------------

PlaitNode* PlaitCell::find_root_node(const std::string& name) const {
  CHECK_N(name == "core");

  auto it = root_nodes.find(name);
  if (it == root_nodes.end()) {
    CHECK_P(false);
    return nullptr;
  }
  else {
    return (*it).second;
  }
}

PlaitNode* PlaitCell::find_leaf_node(const std::string& name) const {
  CHECK_N(name == "core");

  auto it = leaf_nodes.find(name);
  if (it == leaf_nodes.end()) {
    CHECK_P(false);
    return nullptr;
  }
  else {
    return (*it).second;
  }
}

//--------------------------------------------------------------------------------

PlaitNode* PlaitCell::spawn_root_node(PlaitNode* neighbor, uint32_t guid) {
  auto new_root = new PlaitNode();

  char buf[256];
  sprintf_s(buf, 256, "root_%08x", guid);

  new_root->name = buf;
  new_root->pos_new = neighbor->pos_old - dvec2(128, 0);
  new_root->pos_old = neighbor->pos_old - dvec2(128, 0);
  new_root->color   = core_node->color;

  root_nodes[new_root->name] = new_root;
  new_root->plait_cell = this;

  return new_root;
}

PlaitNode* PlaitCell::spawn_leaf_node(PlaitNode* neighbor, uint32_t guid) {
  auto new_leaf = new PlaitNode();

  char buf[256];
  sprintf_s(buf, 256, "leaf_%08x", guid);

  new_leaf->name = buf;
  new_leaf->pos_new = neighbor->pos_old + dvec2(128, 0);
  new_leaf->pos_old = neighbor->pos_old + dvec2(128, 0);
  new_leaf->color   = core_node->color;

  leaf_nodes[new_leaf->name] = new_leaf;
  new_leaf->plait_cell = this;

  return new_leaf;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::clear() {
  for (auto& [tag, plait_cell] : cell_map) {
    delete plait_cell;
  }
  for (auto& [trace_key, plait_trace] : trace_map) {
    delete plait_trace;
  }

  cell_map.clear();
  trace_map.clear();
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::spawn_root_node(PlaitNode* node) {
  node->plait_cell->spawn_root_node(node, new_guid());
}

void Plait::spawn_leaf_node(PlaitNode* node) {
  node->plait_cell->spawn_leaf_node(node, new_guid());
}

//--------------------------------------------------------------------------------

void Plait::delete_roots(PlaitNode* core_node) {
  if (core_node->name != "core") return;
  auto plait_cell = core_node->plait_cell;

  auto dead_leaves = decltype(plait_cell->root_nodes)();
  dead_leaves.swap(plait_cell->root_nodes);

  for (auto& [name, dead_root] : dead_leaves) {
    swap_input_edges(dead_root, plait_cell->core_node);
    check_dead(dead_root);
    delete dead_root;
  }
}

void Plait::delete_leaves(PlaitNode* core_node) {
  if (core_node->name != "core") return;
  auto plait_cell = core_node->plait_cell;

  auto dead_leaves = decltype(plait_cell->leaf_nodes)();
  dead_leaves.swap(plait_cell->leaf_nodes);

  for (auto& [name, dead_leaf] : dead_leaves) {
    swap_output_edges(dead_leaf, plait_cell->core_node);
    check_dead(dead_leaf);
    delete dead_leaf;
  }
}

//--------------------------------------------------------------------------------

void Plait::link_nodes(PlaitNode* node_a, PlaitNode* node_b) {
  for (auto& [trace_key, plait_trace] : trace_map) {

    if (plait_trace->output_node->plait_cell == node_a->plait_cell) {
      if (plait_trace->input_node->plait_cell == node_b->plait_cell) {
        plait_trace->output_node_name = node_a->name;
        plait_trace->output_node = node_a;
        plait_trace->input_node_name = node_b->name;
        plait_trace->input_node = node_b;
      }
    }

    if (plait_trace->output_node->plait_cell == node_b->plait_cell) {
      if (plait_trace->input_node->plait_cell == node_a->plait_cell) {
        plait_trace->output_node_name = node_b->name;
        plait_trace->output_node = node_b;
        plait_trace->input_node_name = node_a->name;
        plait_trace->input_node = node_a;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::swap_input_edges(PlaitNode* old_node, PlaitNode* new_node) {
  CHECK_P(old_node->plait_cell == new_node->plait_cell);

  for (auto& [trace_key, plait_trace] : trace_map) {
    if (plait_trace->input_node == old_node) {
      plait_trace->input_node_name = new_node->name;
      plait_trace->input_node = new_node;
    }
  }
}

void Plait::swap_output_edges(PlaitNode* old_node, PlaitNode* new_node) {
  CHECK_P(old_node->plait_cell == new_node->plait_cell);

  for (auto& [trace_key, plait_trace] : trace_map) {
    if (plait_trace->output_node == old_node) {
      plait_trace->output_node_name = new_node->name;
      plait_trace->output_node = new_node;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::delete_node(PlaitNode* dead_node) {
  CHECK_N(dead_node->name == "core");

  auto plait_cell = dead_node->plait_cell;
  plait_cell->root_nodes.erase(dead_node->name);
  plait_cell->leaf_nodes.erase(dead_node->name);

  swap_input_edges (dead_node, plait_cell->core_node);
  swap_output_edges(dead_node, plait_cell->core_node);
  check_dead(dead_node);
  delete dead_node;
}

//--------------------------------------------------------------------------------

void Plait::check_dead(PlaitNode* dead_leaf) {
  for (auto& [tag, plait_cell] : cell_map) {

    CHECK_P(dead_leaf != plait_cell->core_node);

    for (auto& [name, leaf] : plait_cell->leaf_nodes) {
      CHECK_P(leaf != dead_leaf);
    }
    for (auto& [name, leaf] : plait_cell->root_nodes) {
      CHECK_P(leaf != dead_leaf);
    }
  }

  for (auto& [trace_key, plait_trace] : trace_map) {
    CHECK_P(plait_trace->input_node  != dead_leaf);
    CHECK_P(plait_trace->output_node != dead_leaf);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::save_json(const char* filename) {
  using namespace nlohmann;

  json jroot;

  jroot["cells"]  = cell_map;
  jroot["traces"] = trace_map;
  jroot["labels"] = labels;
  jroot["guid"]   = _guid;

  std::ofstream stream(filename);
  stream << jroot.dump(2);
}

//----------------------------------------


void Plait::save_json(std::ostream& stream) {
  using namespace nlohmann;

  json jroot;
  jroot["cells"]  = cell_map;
  jroot["traces"] = trace_map;
  jroot["labels"] = labels;
  jroot["guid"]   = _guid;

  stream << jroot.dump(2);
}

//--------------------------------------------------------------------------------

void Plait::load_json(const char* filename, DieDB& die_db) {
  std::ifstream stream(filename);
  load_json(stream, die_db);
}

//----------------------------------------

void Plait::load_json(std::istream& stream, DieDB& die_db) {
  using namespace nlohmann;

  CHECK_P(cell_map.empty());

  json jroot;
  stream >> jroot;

  jroot["cells"] .get_to(cell_map);
  jroot["traces"].get_to(trace_map);
  jroot["labels"].get_to(labels);
  jroot["guid"]  .get_to(_guid);

  // Hook up plait_cell pointers.

  for (auto& [tag, plait_cell] : cell_map) {
    auto die_cell = die_db.cell_map[tag];
    CHECK_P(die_cell);
    plait_cell->die_cell = die_cell;
    die_cell->plait_cell = plait_cell;

    plait_cell->core_node->plait_cell = plait_cell;

    for (auto& [name, leaf] : plait_cell->leaf_nodes) {
      leaf->plait_cell = plait_cell;
    }
  }

  // Hook up plait_trace pointers.

  for (auto& [trace_key, plait_trace] : trace_map) {
    auto die_trace = die_db.trace_map[trace_key];
    CHECK_P(die_trace);

    auto output_cell = cell_map[die_trace->output_tag];
    auto input_cell  = cell_map[die_trace->input_tag];

    plait_trace->die_trace = die_trace;
    if (plait_trace->output_node_name == "core") {
      plait_trace->output_node = output_cell->core_node;
    }
    else {
      plait_trace->output_node = output_cell->find_leaf_node(plait_trace->output_node_name);
    }
    plait_trace->input_node  = input_cell->core_node;

    CHECK_P(plait_trace->input_node);
    CHECK_P(plait_trace->output_node);

    plait_trace->output_port_index = output_cell->get_output_index(die_trace->output_port);
    plait_trace->input_port_index  = input_cell->get_input_index(die_trace->input_port);

    die_trace->plait_trace = plait_trace;
  }

  // Fill in missing cells.

  for (auto& [tag, die_cell] : die_db.cell_map) {
    if (die_cell->plait_cell) continue;

    printf("Did not load node for tag \"%s\", creating default node\n", tag.c_str());

    auto core_node = new PlaitNode();
    core_node->name = "core";

    auto new_cell = new PlaitCell();
    new_cell->die_cell = die_cell;
    new_cell->core_node = core_node;

    die_cell->plait_cell = new_cell;
    cell_map[tag] = new_cell;
  }

  // Fill in missing edges.

  for (auto& [trace_key, die_trace] : die_db.trace_map) {
    if (die_trace->plait_trace) continue;

    printf("Did not load plait trace for die trace \"%s\", creating default trace\n", die_trace->to_key().c_str());

    auto output_cell = cell_map[die_trace->output_tag];
    auto output_node = output_cell->core_node;
    auto output_port_index = output_cell->get_output_index(die_trace->output_port);

    auto input_cell = cell_map[die_trace->input_tag];
    auto input_node = input_cell->core_node;
    auto input_port_index = input_cell->get_input_index(die_trace->input_port);

    CHECK_P(output_cell);
    CHECK_P(output_node);
    CHECK_P(output_port_index >= 0);

    CHECK_P(input_cell);
    CHECK_P(input_node);
    CHECK_P(input_port_index >= 0);

    auto plait_trace = new PlaitTrace();

    plait_trace->output_cell_name = output_cell->tag();
    plait_trace->output_node_name = output_node->name;

    plait_trace->input_cell_name = input_node->name;
    plait_trace->input_node_name = input_node->name;

    plait_trace->die_trace = die_trace;
    plait_trace->output_node = output_node;
    plait_trace->input_node = input_node;

    plait_trace->output_port_index = output_cell->get_input_index(die_trace->output_port);
    plait_trace->input_port_index  = input_cell->get_output_index(die_trace->input_port);

    die_trace->plait_trace = plait_trace;

    trace_map[die_trace->to_key()] = plait_trace;
  }

  // Sanity checks

  for (auto& [tag, plait_cell] : cell_map) {
    // Every cell should have a core node.
    CHECK_P(plait_cell->core_node);
    CHECK_N(plait_cell->core_node->name.empty());
    CHECK_P(plait_cell->core_node->plait_cell == plait_cell);

    // Every node should have a link to the cell.
    for (auto& [name, root] : plait_cell->root_nodes) {
      CHECK_P(root);
      CHECK_N(root->name.empty());
      CHECK_P(root->plait_cell == plait_cell);
    }
    for (auto& [name, leaf] : plait_cell->leaf_nodes) {
      CHECK_P(leaf);
      CHECK_N(leaf->name.empty());
      CHECK_P(leaf->plait_cell == plait_cell);
    }
  }

  for (auto& [trace_key, plait_trace] : trace_map) {
    CHECK_P(plait_trace->die_trace);
    CHECK_P(plait_trace->output_node);
    CHECK_P(plait_trace->input_node);

    CHECK_P(plait_trace->output_cell_name == plait_trace->die_trace->output_tag);
    CHECK_P(plait_trace->output_node_name == plait_trace->output_node->name);

    CHECK_P(plait_trace->input_cell_name == plait_trace->die_trace->input_tag);
    CHECK_P(plait_trace->input_node_name == plait_trace->input_node->name);

    CHECK_P(plait_trace->die_trace->plait_trace == plait_trace);
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
