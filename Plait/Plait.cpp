#include "Plait/Plait.h"

#include <iostream>
#include <fstream>

#include "json/single_include/nlohmann/json.hpp"

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

const char* Node::tag() const {
  if (cell) {
    return cell->tag.c_str();
  }
  else {
    return "<no_tag>";
  }
}

const char* Node::name() const {
  if (cell) {
    if (cell->name.empty()) {
      return cell->tag.c_str();
    }
    else {
      return cell->name.c_str();
    }
  }
  else {
    return "<no_cell>";
  }
}

const char* Node::gate() const {
  if (cell) {
    return cell->gate.c_str();
  }
  else {
    return "<no_gate>";
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

bool NodeInstance::anchored_to(NodeInstance* target) {
  for (auto cursor = anchor; cursor; cursor = cursor->anchor) {
    if (cursor == target) return true;
  }
  return false;
}

void NodeInstance::set_anchor(NodeInstance* new_anchor) {
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
    for(auto p : prev) {
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

  json root;

  for (auto& [tag, node] : node_map) {
    auto& jnode = root[tag];
    node->nodes[0].commit_pos();

    jnode["locked"]     = node->nodes[0].locked;
    jnode["pos_rel_x"]  = node->nodes[0].pos_rel.x;
    jnode["pos_rel_y"]  = node->nodes[0].pos_rel.y;
    jnode["pos_abs_x"]  = node->nodes[0].pos_abs.x;
    jnode["pos_abs_y"]  = node->nodes[0].pos_abs.y;
    if (node->nodes[0].anchor) jnode["anchor_tag"] = node->nodes[0].anchor->cell->tag;
  }

  std::ofstream(filename) << root.dump(2);
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void fix_node_rel_abs(NodeInstance* n) {
  if (n->mark != -100) return;

  if (n->anchor) {
    fix_node_rel_abs(n->anchor);
    n->pos_abs = n->pos_rel + n->anchor->pos_abs;
  }
  else {
    n->pos_abs = n->pos_rel;
  }
  n->mark = 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

void Plait::load_json(const char* filename, CellDB& cell_db) {
  printf("Loading plait %s\n", filename);
  using namespace nlohmann;

  CHECK_P(node_map.empty());

  json root;
  std::ifstream(filename) >> root;

  for (auto& it : root.items()) {
    auto& tag = it.key();
    auto& jnode = it.value();

    auto cell = cell_db.tag_to_cell[tag];
    if (cell == nullptr) {
      printf("Did not recognize cell tag %s\n", tag.c_str());
      continue;
    }

    auto node = new Node(cell);
    node_map[tag] = node;

    node->nodes[0].locked    = jnode.value("locked", false);
    node->nodes[0].pos_rel.x = jnode.value("pos_rel_x", 0.0);
    node->nodes[0].pos_rel.y = jnode.value("pos_rel_y", 0.0);
    node->nodes[0].pos_abs.x = jnode.value("pos_abs_x", 0.0);
    node->nodes[0].pos_abs.y = jnode.value("pos_abs_y", 0.0);
  }

#if 1
  // Check for missing tags
  for (auto& [tag, cell] : cell_db.tag_to_cell) {
    if (node_map.count(tag) == 0) {
      printf("Did not load node for tag \"%s\", creating placeholder\n", tag.c_str());
      node_map[tag] = new Node(cell);
    }
  }
#endif

  // Connect anchors
  for (auto& [tag, jnode] : root.items()) {
    if (!jnode["anchor_tag"].is_string()) continue;
    const auto& anchor_tag = jnode["anchor_tag"].get<std::string>();

    if (tag == anchor_tag) {
      printf("node %s anchored to itself?\n", tag.c_str());
    }

    auto node = node_map[tag];
    auto anchor = node_map[anchor_tag];

    if (node && anchor) {
      node->nodes[0].anchor = &anchor->nodes[0];
    }
    else if (node == nullptr) {
      printf("bad tag %s\n", tag.c_str());
    }
    else if (anchor == nullptr) {
      printf("bad anchor tag %s\n", anchor_tag.c_str());
    }
  }

#if 0
  // Fix abs/rel
  //for (auto& [tag, node] : node_map) {
  //  printf("%-22s (%12.2f %12.2f) (%12.2f %12.2f)\n", node->tag(), node->pos_rel.x, node->pos_rel.y, node->pos_abs.x, node->pos_abs.y);
  //}

  for (auto& [tag, node] : node_map) {
    node->mark = -100;
  }
  for (auto& [tag, node] : node_map) {
    fix_node_rel_abs(node);
  }

  //for (auto& [tag, node] : node_map) {
  //  printf("%-22s (%12.2f %12.2f) (%12.2f %12.2f)\n", node->tag(), node->pos_rel.x, node->pos_rel.y, node->pos_abs.x, node->pos_abs.y);
  //}
#endif

  // Connect ports
  for (auto& [tag, node] : node_map) {
    auto arg_count = node->nodes[0].cell->args.size();
    for (auto i = 0; i < arg_count; i++) {
      auto& arg = node->nodes[0].cell->args[i];
      auto prev = node_map[arg.tag];

      if (prev == nullptr) {
        printf("Did not recognize arg tag %s\n", tag.c_str());
      }
      else {
        node->prev_node.push_back(prev);
        node->prev_port.push_back(arg.port);
        prev->next.push_back(node);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
