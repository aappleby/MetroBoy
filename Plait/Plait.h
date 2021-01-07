#pragma once

#include "Plait/CellDB.h"
#include "glm/glm/glm.hpp"

#include <vector>

using namespace glm;

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Node {

  bool locked = 0;
  dvec2 pos_abs = {0,0};
  dvec2 pos_rel = {0,0};
  Node* anchor = nullptr;
  bool pinned = 1;
  const Cell* cell = nullptr;
  int  mark = 0;
  bool ghost = 0;
  bool selected = 0; // need this because we don't want a log(n) lookup per node per frame...
  dvec2 spring_force = {0,0};

  bool  anchored() { return anchor != nullptr; }
  bool  anchored_to(Node* target);
  const Node* get_anchor() { return anchor; }
  void  set_anchor(Node* new_anchor);

  void toggle_ghost()  { ghost = !ghost; }

  dvec2 get_pos_abs_new() const {
    if (!pinned) {
      return pos_abs;
    }
    else {
      return get_pos_anchor_abs_new() + pos_rel;
    }
  }

  dvec2 get_pos_rel_new() const {
    if (!pinned) {
      return pos_abs - get_pos_anchor_abs_new();
    }
    else {
      return pos_rel;
    }
  }

  dvec2 get_pos_abs_old() const {
    return get_pos_anchor_abs_old() + pos_rel;
  }

  dvec2 get_pos_rel_old() const {
    return pos_rel;
  }


  dvec2 get_pos_anchor_abs_new() const {
    return anchor ? anchor->get_pos_abs_new() : dvec2(0,0);
  }

  dvec2 get_pos_anchor_abs_old() const {
    return anchor ? anchor->get_pos_abs_old() : dvec2(0,0);
  }

  void move(dvec2 delta) {
    unpin();
    pos_abs += delta;
  }

  void set_pos_abs_new(dvec2 p) {
    pos_abs = p;
    pinned = false;
  }

  void commit_pos() {
    if (!pinned) {
      printf("commit_pos\n");
      pos_rel = pos_abs - get_pos_anchor_abs_new();
      pinned = true;
    }
  }

  void revert_pos() {
    if (!pinned) {
      printf("revert_pos\n");
      pos_abs = pos_rel + get_pos_anchor_abs_new();
      pinned = true;
    }
  }

  void unpin() {
    if (pinned) {
      pos_abs = pos_rel + get_pos_anchor_abs_new();
      pinned = false;
    }
  }

  void toggle_locked() { locked = !locked; }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

struct NodeGroup {
  NodeGroup(const Cell* _cell) {
    cell = _cell;
    nodes.resize(1);
    nodes[0] = new Node();
    nodes[0]->cell = _cell;
  }

  const char* tag() const;
  const char* name() const;
  const char* gate() const;

  //----------------------------------------
  // Serialized state

  std::vector<Node*> nodes;

  //----------------------------------------
  // State from cell db

  const Cell* cell = nullptr;
  std::vector<NodeGroup*>  prev_group;
  std::vector<std::string> prev_port;
  std::vector<NodeGroup*>  next_group;

  //----------------------------------------
  // Sim/UI state

  uint32_t color = 0xFFFF00FF;
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------

struct Plait {
  void save_json(const char* filename);
  void load_json(const char* filename, CellDB& cell_db);

  std::map<std::string, NodeGroup*> node_map;

  NodeGroup* get_or_create_node(const std::string& tag, CellDB& cell_db) {
    auto cell = cell_db.tag_to_cell[tag];
    if (!cell) {
      printf("Tag \"%s\" not in cell db\n", tag.c_str());
      return nullptr;
    }

    auto node = node_map[tag];
    if (node) return node;

    node = new NodeGroup(cell);
    node_map[tag] = node;
    return node;
  }
};

//----------------------------------------------------------------------------------------------------------------------------------------------------------------
