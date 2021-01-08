#include "Plait/Plait.h"

#include <iostream>
#include <fstream>

#include "json/single_include/nlohmann/json.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool PlaitNode::anchored_to(PlaitNode* target) {
  for (auto cursor = anchor; cursor; cursor = cursor->anchor) {
    if (cursor == target) return true;
  }
  return false;
}

void PlaitNode::set_anchor(PlaitNode* new_anchor) {
  CHECK_P(new_anchor == nullptr || pinned);
  CHECK_P(new_anchor == nullptr || new_anchor->pinned);

  // Ignore invalid anchors, or anchors that would create a loop.
  if (this == new_anchor) return;
  if (anchor == new_anchor) return;
  if (new_anchor && new_anchor->anchored_to(this)) return;

  // Unlink from old anchor.
  if (anchor) {
    pos_rel += anchor->get_pos_abs_old();
    anchor = nullptr;
  }

  // Link to new anchor.
  if (new_anchor) {
    pos_rel -= new_anchor->get_pos_abs_old();
    anchor = new_anchor;
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

#if 0
  void update_rank1() {
    mark = 1;
    rank = 0;
    for(auto p : prev) {node
      if (p->mark) continue;
      if (p->rank == -1) p->update_rank1();
      if (p->rank >= rank) {
        rank = p->rank + 1;
      }
    }
    mark = 0;
  }

  void update_rank2() {
    if (prev.empty() && next.empty()) {
      rank = 0;
    }
    else if (prev.empty()) {
      int min_next = 1000000;
      for (auto n : next) if (n->rank < min_next) min_next = n->rank;
      rank = min_next - 1;
    }
    else if (next.empty()) {
      int max_prev = -1;
      for (auto p : prev) if (p->rank > max_prev) max_prev = p->rank;
      rank = max_prev + 1;
    }
    else {
      int max_prev = -1;
      for (auto p : prev) if (p->rank > max_prev) max_prev = p->rank;
      int min_next = 1000000;
      for (auto n : next) if (n->rank < min_next) min_next = n->rank;

      rank = (max_prev + min_next) / 2;
    }
  }
#endif

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::save_json(const char* filename) {
  printf("Saving plait %s\n", filename);
  using namespace nlohmann;

  for (auto& [tag, cell] : tag_to_cell) {
    for (auto node : cell->nodes) node->commit_pos();
  }

  json jroot;

  auto& jcells = jroot["cells"];

  for (auto& [tag, plait_cell] : tag_to_cell) {
    auto& jcell = jcells[tag];
    jcell["name"]       = plait_cell->nodes[0]->name;
    jcell["locked"]     = plait_cell->nodes[0]->locked;
    jcell["pos_rel_x"]  = plait_cell->nodes[0]->pos_rel.x;
    jcell["pos_rel_y"]  = plait_cell->nodes[0]->pos_rel.y;
    jcell["pos_abs_x"]  = plait_cell->nodes[0]->pos_abs.x;
    jcell["pos_abs_y"]  = plait_cell->nodes[0]->pos_abs.y;
    if (plait_cell->nodes[0]->anchor) jcell["anchor_tag"] = plait_cell->nodes[0]->anchor->cell->die_cell->tag;

    auto& jnodes = jcell["nodes"];
    for (auto node : plait_cell->nodes) {
      auto& jnode = jnodes[node->name];
      jnode["locked"]     = plait_cell->nodes[0]->locked;
      jnode["pos_rel_x"]  = plait_cell->nodes[0]->pos_rel.x;
      jnode["pos_rel_y"]  = plait_cell->nodes[0]->pos_rel.y;
      jnode["pos_abs_x"]  = plait_cell->nodes[0]->pos_abs.x;
      jnode["pos_abs_y"]  = plait_cell->nodes[0]->pos_abs.y;
      if (plait_cell->nodes[0]->anchor) jnode["anchor_tag"] = plait_cell->nodes[0]->anchor->cell->die_cell->tag;
    }
  }

  std::ofstream(filename) << jroot.dump(2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::load_json(const char* filename, DieDB& die_db) {
  printf("Loading plait %s\n", filename);
  using namespace nlohmann;

  CHECK_P(tag_to_cell.empty());

  json jroot;
  std::ifstream(filename) >> jroot;

  auto& jcells = jroot["cells"];

  for (auto& [tag, jcell] : jcells.items()) {

    auto die_cell = die_db.tag_to_cell[tag];
    if (die_cell == nullptr) {
      printf("Did not recognize cell tag %s\n", tag.c_str());
      continue;
    }

    auto plait_cell = new PlaitCell(die_cell);
    tag_to_cell[tag] = plait_cell;

    for (auto& [name, jnode] : jcell["nodes"].items()) {
      auto node = plait_cell->add_node(name);
      node->locked     = jnode.value("locked", false);
      node->pos_rel.x  = jnode.value("pos_rel_x", 0.0);
      node->pos_rel.y  = jnode.value("pos_rel_y", 0.0);
      node->pos_abs.x  = jnode.value("pos_abs_x", 0.0);
      node->pos_abs.y  = jnode.value("pos_abs_y", 0.0);
      node->anchor_tag = jnode.value("anchor_tag", "");
    }
  }

  // Check for missing tags
  for (auto& [tag, die_cell] : die_db.tag_to_cell) {
    if (tag_to_cell.count(tag) == 0) {
      printf("Did not load node for tag \"%s\", creating placeholder\n", tag.c_str());
      {
        auto group = new PlaitCell(die_cell);
        tag_to_cell[tag] = group;
        group->add_node("default");
      }
    }
  }

  // Connect anchors

  for (auto& [tag, plait_cell] : tag_to_cell) {
    for (auto& node : plait_cell->nodes) {
      if (node->anchor_tag.empty()) continue;
      auto anchor_cell = tag_to_cell[node->anchor_tag];

      if (anchor_cell) {
        // FIXME anchor path
        node->anchor = anchor_cell->nodes[0];
      }
      else {
        printf("bad anchor tag %s\n", node->anchor_tag.c_str());
      }
    }
  }

  // Connect ports
  for (auto& [tag, plait_cell] : tag_to_cell) {
    auto node = plait_cell->nodes[0];
    auto arg_count = plait_cell->die_cell->args.size();

    for (auto i = 0; i < arg_count; i++) {
      auto& arg = plait_cell->die_cell->args[i];
      auto prev_plait_cell = tag_to_cell[arg.tag];
      auto prev_node = prev_plait_cell->nodes[0];

      if (prev_plait_cell == nullptr) {
        printf("Did not recognize arg tag %s\n", tag.c_str());
      }
      else {
        plait_cell->prev_cells.push_back(prev_plait_cell);
        node->prev_nodes.push_back(prev_node);
        node->prev_ports.push_back(arg.port);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
