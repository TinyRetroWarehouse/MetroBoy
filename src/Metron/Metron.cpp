#include "Platform.h"
#include "MtModule.h"
#include "MtModLibrary.h"
#include "MtCursor.h"

#pragma warning(disable:4996) // unsafe fopen()

//------------------------------------------------------------------------------

int main(int argc, char** argv) {

  std::vector<std::string> inputs;
  for (int i = 1; i < argc; i++) {
    if (argv[i][0] == '-') {
    } else {
      inputs.push_back(argv[i]);
    }
  }

  MtModLibrary lib;
  for (auto& input : inputs) {
    lib.load(input.c_str(), (input + ".sv").c_str());
  }

  for (auto module : lib.modules)
  {
    MtCursor cursor(module, module->out_file);

    cursor.emit("//--------------------------------------------------------------------------------\n");
    cursor.emit("// MODULE:       ", module);

    cursor.emit("%s\n", module->module_class.node_to_name().c_str());

    cursor.emit("// MODULEPARAMS: ");
    for (auto f : module->moduleparams) {
      cursor.emit("%s, ", f.node_to_name().c_str());
    }
    cursor.emit("\n");

    cursor.emit("// INPUTS:       ");
    for (auto f : module->inputs) cursor.emit("%s, ", f.node_to_name().c_str());
    cursor.emit("\n");

    cursor.emit("// OUTPUTS:      ");
    for (auto f : module->outputs) cursor.emit("%s, ", f.node_to_name().c_str());
    cursor.emit("\n");

    cursor.emit("// LOCALPARAMS:  ");
    for (auto f : module->localparams) cursor.emit("%s, ", f.node_to_name().c_str());
    cursor.emit("\n");

    cursor.emit("// FIELDS:       ");
    for (auto f : module->fields) {
      cursor.emit("%s, ", f.node_to_name().c_str());
    }
    cursor.emit("\n");

    cursor.emit("// SUBMODULES:   ");
    for (auto f : module->submodules) cursor.emit("%s, ", f.node_to_name().c_str());
    cursor.emit("\n");

    cursor.emit("// TASKS:        ");
    for (auto f : module->tasks) cursor.emit("%s, ", f.node_to_name().c_str());
    cursor.emit("\n");

    cursor.emit("// FUNCTIONS:    ");
    for (auto f : module->functions) cursor.emit("%s, ", f.node_to_name().c_str());
    cursor.emit("\n");

    cursor.cursor = module->source;
    cursor.emit_dispatch(module->root);
    printf("\n");
  }

  return 0;
}

//------------------------------------------------------------------------------
