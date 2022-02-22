#pragma once
#include "Platform.h"

#include "MtNode.h"
#include "MtField.h"

//------------------------------------------------------------------------------

struct MtModule {

  MtModule();
  ~MtModule();

  void load(const std::string& full_path);
  void print_error(MtNode n, const char* fmt, ...);

  // Identifier lookup

  MtNode get_by_id(std::vector<MtNode>& handles, MtNode id);
  MtNode get_task_by_id(MtNode id)     { return get_by_id(tasks, id); }
  MtNode get_function_by_id(MtNode id) { return get_by_id(funcs, id);}

  MtField get_by_id(std::vector<MtField>& handles, MtNode id);
  MtField get_input_by_id(MtNode id)  { return get_by_id(inputs, id); }
  MtField get_output_by_id(MtNode id) { return get_by_id(outputs, id); }
  MtField get_field_by_id(MtNode id)  { return get_by_id(outputs, id); }

  // Scanner
  
  void find_module();
  void collect_modparams();
  void collect_localparams();

  void collect_functions();

  void collect_ports();

  void collect_fields();

  void dedup_inputs();

  // Rule checker

  void check_dirty_ticks();
  void check_dirty_tocks();
  void check_dirty_read(MtNode n, bool is_seq, std::set<MtField>& dirty_fields, int depth);
  void check_dirty_write(MtNode n, bool is_seq, std::set<MtField>& dirty_fields, int depth);
  void check_dirty_dispatch(MtNode n, bool is_seq, std::set<MtField>& dirty_fields, int depth);
  void check_dirty_assign(MtNode n, bool is_seq, std::set<MtField>& dirty_fields, int depth);
  void check_dirty_if(MtNode n, bool is_seq, std::set<MtField>& dirty_fields, int depth);
  void check_dirty_call(MtNode n, bool is_seq, std::set<MtField>& dirty_fields, int depth);
  void check_dirty_switch(MtNode n, bool is_seq, std::set<MtField>& dirty_fields, int depth);

  //----------

  MtModLibrary* lib;
  std::string full_path;

  blob src_blob;
  bool use_utf8_bom = false;

  const char* source = nullptr;
  const char* source_end = nullptr;
  const TSLanguage* lang = nullptr;
  TSParser* parser = nullptr;
  TSTree* tree = nullptr;

  std::string mod_name;

  MtTranslationUnit   mod_root;
  MtTemplateDecl      mod_template;
  MtStructSpecifier   mod_class;
  MtTemplateParamList mod_param_list;

  std::vector<MtNode> modparams;
  std::vector<MtNode> localparams;
  std::vector<MtNode> enums;

  std::vector<MtNode> inits;
  std::vector<MtNode> ticks;
  std::vector<MtNode> tocks;
  std::vector<MtNode> tasks;
  std::vector<MtNode> funcs;

  std::vector<MtField> inputs;
  std::vector<MtField> outputs;
  std::vector<MtField> fields;
  std::vector<MtNode>  submodules;
};

//------------------------------------------------------------------------------
