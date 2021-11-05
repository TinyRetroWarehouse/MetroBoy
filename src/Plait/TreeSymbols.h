#pragma once

enum {
  field_alternative = 1,
  field_argument = 2,
  field_arguments = 3,
  field_base = 4,
  field_body = 5,
  field_captures = 6,
  field_condition = 7,
  field_consequence = 8,
  field_declarator = 9,
  field_default_type = 10,
  field_default_value = 11,
  field_designator = 12,
  field_directive = 13,
  field_field = 14,
  field_function = 15,
  field_index = 16,
  field_initializer = 17,
  field_label = 18,
  field_left = 19,
  field_length = 20,
  field_message = 21,
  field_name = 22,
  field_operator = 23,
  field_parameters = 24,
  field_path = 25,
  field_pattern = 26,
  field_placement = 27,
  field_prefix = 28,
  field_right = 29,
  field_scope = 30,
  field_size = 31,
  field_type = 32,
  field_update = 33,
  field_value = 34,
};

enum {
  sym_identifier = 1,
  aux_sym_preproc_include_token1 = 2,
  anon_sym_LF = 3,
  aux_sym_preproc_def_token1 = 4,
  anon_sym_LPAREN = 5,
  anon_sym_DOT_DOT_DOT = 6,
  anon_sym_COMMA = 7,
  anon_sym_RPAREN = 8,
  aux_sym_preproc_if_token1 = 9,
  aux_sym_preproc_if_token2 = 10,
  aux_sym_preproc_ifdef_token1 = 11,
  aux_sym_preproc_ifdef_token2 = 12,
  aux_sym_preproc_else_token1 = 13,
  aux_sym_preproc_elif_token1 = 14,
  sym_preproc_directive = 15,
  sym_preproc_arg = 16,
  anon_sym_LPAREN2 = 17,
  anon_sym_defined = 18,
  anon_sym_BANG = 19,
  anon_sym_TILDE = 20,
  anon_sym_DASH = 21,
  anon_sym_PLUS = 22,
  anon_sym_STAR = 23,
  anon_sym_SLASH = 24,
  anon_sym_PERCENT = 25,
  anon_sym_PIPE_PIPE = 26,
  anon_sym_AMP_AMP = 27,
  anon_sym_PIPE = 28,
  anon_sym_CARET = 29,
  anon_sym_AMP = 30,
  anon_sym_EQ_EQ = 31,
  anon_sym_BANG_EQ = 32,
  anon_sym_GT = 33,
  anon_sym_GT_EQ = 34,
  anon_sym_LT_EQ = 35,
  anon_sym_LT = 36,
  anon_sym_LT_LT = 37,
  anon_sym_GT_GT = 38,
  anon_sym_SEMI = 39,
  anon_sym_typedef = 40,
  anon_sym_extern = 41,
  anon_sym___attribute__ = 42,
  anon_sym_COLON_COLON = 43,
  anon_sym_LBRACK_LBRACK = 44,
  anon_sym_RBRACK_RBRACK = 45,
  anon_sym___declspec = 46,
  anon_sym___based = 47,
  anon_sym___cdecl = 48,
  anon_sym___clrcall = 49,
  anon_sym___stdcall = 50,
  anon_sym___fastcall = 51,
  anon_sym___thiscall = 52,
  anon_sym___vectorcall = 53,
  sym_ms_restrict_modifier = 54,
  sym_ms_unsigned_ptr_modifier = 55,
  sym_ms_signed_ptr_modifier = 56,
  anon_sym__unaligned = 57,
  anon_sym___unaligned = 58,
  anon_sym_LBRACE = 59,
  anon_sym_RBRACE = 60,
  anon_sym_LBRACK = 61,
  anon_sym_RBRACK = 62,
  anon_sym_EQ = 63,
  anon_sym_static = 64,
  anon_sym_register = 65,
  anon_sym_inline = 66,
  anon_sym_thread_local = 67,
  anon_sym_const = 68,
  anon_sym_volatile = 69,
  anon_sym_restrict = 70,
  anon_sym__Atomic = 71,
  anon_sym_mutable = 72,
  anon_sym_constexpr = 73,
  anon_sym_signed = 74,
  anon_sym_unsigned = 75,
  anon_sym_long = 76,
  anon_sym_short = 77,
  sym_primitive_type = 78,
  anon_sym_enum = 79,
  anon_sym_class = 80,
  anon_sym_struct = 81,
  anon_sym_union = 82,
  anon_sym_COLON = 83,
  anon_sym_if = 84,
  anon_sym_else = 85,
  anon_sym_switch = 86,
  anon_sym_case = 87,
  anon_sym_default = 88,
  anon_sym_while = 89,
  anon_sym_do = 90,
  anon_sym_for = 91,
  anon_sym_return = 92,
  anon_sym_break = 93,
  anon_sym_continue = 94,
  anon_sym_goto = 95,
  anon_sym_QMARK = 96,
  anon_sym_STAR_EQ = 97,
  anon_sym_SLASH_EQ = 98,
  anon_sym_PERCENT_EQ = 99,
  anon_sym_PLUS_EQ = 100,
  anon_sym_DASH_EQ = 101,
  anon_sym_LT_LT_EQ = 102,
  anon_sym_GT_GT_EQ = 103,
  anon_sym_AMP_EQ = 104,
  anon_sym_CARET_EQ = 105,
  anon_sym_PIPE_EQ = 106,
  anon_sym_DASH_DASH = 107,
  anon_sym_PLUS_PLUS = 108,
  anon_sym_sizeof = 109,
  anon_sym_DOT = 110,
  anon_sym_DASH_GT = 111,
  sym_number_literal = 112,
  anon_sym_L_SQUOTE = 113,
  anon_sym_u_SQUOTE = 114,
  anon_sym_U_SQUOTE = 115,
  anon_sym_u8_SQUOTE = 116,
  anon_sym_SQUOTE = 117,
  aux_sym_char_literal_token1 = 118,
  anon_sym_L_DQUOTE = 119,
  anon_sym_u_DQUOTE = 120,
  anon_sym_U_DQUOTE = 121,
  anon_sym_u8_DQUOTE = 122,
  anon_sym_DQUOTE = 123,
  aux_sym_string_literal_token1 = 124,
  sym_escape_sequence = 125,
  sym_system_lib_string = 126,
  sym_true = 127,
  sym_false = 128,
  sym_null = 129,
  sym_comment = 130,
  anon_sym_decltype = 131,
  anon_sym_final = 132,
  anon_sym_override = 133,
  anon_sym_virtual = 134,
  anon_sym_explicit = 135,
  anon_sym_public = 136,
  anon_sym_private = 137,
  anon_sym_protected = 138,
  sym_auto = 139,
  anon_sym_typename = 140,
  anon_sym_template = 141,
  anon_sym_GT2 = 142,
  anon_sym_operator = 143,
  anon_sym_delete = 144,
  anon_sym_friend = 145,
  anon_sym_noexcept = 146,
  anon_sym_throw = 147,
  anon_sym_namespace = 148,
  anon_sym_using = 149,
  anon_sym_static_assert = 150,
  anon_sym_co_return = 151,
  anon_sym_co_yield = 152,
  anon_sym_try = 153,
  anon_sym_catch = 154,
  anon_sym_co_await = 155,
  anon_sym_new = 156,
  anon_sym_DASH_GT_STAR = 157,
  anon_sym_LPAREN_RPAREN = 158,
  anon_sym_LBRACK_RBRACK = 159,
  anon_sym_DQUOTE_DQUOTE = 160,
  sym_this = 161,
  sym_nullptr = 162,
  sym_literal_suffix = 163,
  sym_raw_string_literal = 164,
  sym_translation_unit = 165,
  sym_preproc_include = 166,
  sym_preproc_def = 167,
  sym_preproc_function_def = 168,
  sym_preproc_params = 169,
  sym_preproc_call = 170,
  sym_preproc_if = 171,
  sym_preproc_ifdef = 172,
  sym_preproc_else = 173,
  sym_preproc_elif = 174,
  sym_preproc_if_in_field_declaration_list = 175,
  sym_preproc_ifdef_in_field_declaration_list = 176,
  sym_preproc_else_in_field_declaration_list = 177,
  sym_preproc_elif_in_field_declaration_list = 178,
  sym__preproc_expression = 179,
  sym_preproc_parenthesized_expression = 180,
  sym_preproc_defined = 181,
  sym_preproc_unary_expression = 182,
  sym_preproc_call_expression = 183,
  sym_preproc_argument_list = 184,
  sym_preproc_binary_expression = 185,
  sym_function_definition = 186,
  sym_declaration = 187,
  sym_type_definition = 188,
  sym__declaration_modifiers = 189,
  sym__declaration_specifiers = 190,
  sym_linkage_specification = 191,
  sym_attribute_specifier = 192,
  sym_attribute = 193,
  sym_attribute_declaration = 194,
  sym_ms_declspec_modifier = 195,
  sym_ms_based_modifier = 196,
  sym_ms_call_modifier = 197,
  sym_ms_unaligned_ptr_modifier = 198,
  sym_ms_pointer_modifier = 199,
  sym_declaration_list = 200,
  sym__declarator = 201,
  sym__field_declarator = 202,
  sym__type_declarator = 203,
  sym__abstract_declarator = 204,
  sym_parenthesized_declarator = 205,
  sym_parenthesized_field_declarator = 206,
  sym_parenthesized_type_declarator = 207,
  sym_abstract_parenthesized_declarator = 208,
  sym_attributed_declarator = 209,
  sym_attributed_field_declarator = 210,
  sym_attributed_type_declarator = 211,
  sym_pointer_declarator = 212,
  sym_pointer_field_declarator = 213,
  sym_pointer_type_declarator = 214,
  sym_abstract_pointer_declarator = 215,
  sym_function_declarator = 216,
  sym_function_field_declarator = 217,
  sym_function_type_declarator = 218,
  sym_abstract_function_declarator = 219,
  sym_array_declarator = 220,
  sym_array_field_declarator = 221,
  sym_array_type_declarator = 222,
  sym_abstract_array_declarator = 223,
  sym_init_declarator = 224,
  sym_compound_statement = 225,
  sym_storage_class_specifier = 226,
  sym_type_qualifier = 227,
  sym__type_specifier = 228,
  sym_sized_type_specifier = 229,
  sym_enum_specifier = 230,
  sym_enumerator_list = 231,
  sym_struct_specifier = 232,
  sym_union_specifier = 233,
  sym_field_declaration_list = 234,
  sym__field_declaration_list_item = 235,
  sym_field_declaration = 236,
  sym_bitfield_clause = 237,
  sym_enumerator = 238,
  sym_parameter_list = 239,
  sym_parameter_declaration = 240,
  sym_attributed_statement = 241,
  sym_attributed_non_case_statement = 242,
  sym_labeled_statement = 243,
  sym_expression_statement = 244,
  sym_if_statement = 245,
  sym_switch_statement = 246,
  sym_case_statement = 247,
  sym_while_statement = 248,
  sym_do_statement = 249,
  sym_for_statement = 250,
  sym_return_statement = 251,
  sym_break_statement = 252,
  sym_continue_statement = 253,
  sym_goto_statement = 254,
  sym__expression = 255,
  sym_comma_expression = 256,
  sym_conditional_expression = 257,
  sym_assignment_expression = 258,
  sym_pointer_expression = 259,
  sym_unary_expression = 260,
  sym_binary_expression = 261,
  sym_update_expression = 262,
  sym_cast_expression = 263,
  sym_type_descriptor = 264,
  sym_sizeof_expression = 265,
  sym_subscript_expression = 266,
  sym_call_expression = 267,
  sym_argument_list = 268,
  sym_field_expression = 269,
  sym_compound_literal_expression = 270,
  sym_parenthesized_expression = 271,
  sym_initializer_list = 272,
  sym_initializer_pair = 273,
  sym_subscript_designator = 274,
  sym_field_designator = 275,
  sym_char_literal = 276,
  sym_concatenated_string = 277,
  sym_string_literal = 278,
  sym__empty_declaration = 279,
  sym_decltype = 280,
  sym_class_specifier = 281,
  sym__class_name = 282,
  sym_virtual_specifier = 283,
  sym_virtual_function_specifier = 284,
  sym_explicit_function_specifier = 285,
  sym_base_class_clause = 286,
  sym__enum_base_clause = 287,
  sym_dependent_type = 288,
  sym_template_declaration = 289,
  sym_template_instantiation = 290,
  sym_template_parameter_list = 291,
  sym_type_parameter_declaration = 292,
  sym_variadic_type_parameter_declaration = 293,
  sym_optional_type_parameter_declaration = 294,
  sym_template_template_parameter_declaration = 295,
  sym_optional_parameter_declaration = 296,
  sym_variadic_parameter_declaration = 297,
  sym_variadic_declarator = 298,
  sym_variadic_reference_declarator = 299,
  sym_operator_cast = 300,
  sym_field_initializer_list = 301,
  sym_field_initializer = 302,
  sym_inline_method_definition = 303,
  sym__constructor_specifiers = 304,
  sym_operator_cast_definition = 305,
  sym_operator_cast_declaration = 306,
  sym_constructor_or_destructor_definition = 307,
  sym_constructor_or_destructor_declaration = 308,
  sym_default_method_clause = 309,
  sym_delete_method_clause = 310,
  sym_friend_declaration = 311,
  sym_access_specifier = 312,
  sym_reference_declarator = 313,
  sym_reference_field_declarator = 314,
  sym_abstract_reference_declarator = 315,
  sym_structured_binding_declarator = 316,
  sym_ref_qualifier = 317,
  sym_trailing_return_type = 318,
  sym_noexcept = 319,
  sym_throw_specifier = 320,
  sym_template_type = 321,
  sym_template_method = 322,
  sym_template_function = 323,
  sym_template_argument_list = 324,
  sym_namespace_definition = 325,
  sym_namespace_definition_name = 326,
  sym_using_declaration = 327,
  sym_alias_declaration = 328,
  sym_static_assert_declaration = 329,
  sym_condition_clause = 330,
  sym_condition_declaration = 331,
  sym_for_range_loop = 332,
  sym_co_return_statement = 333,
  sym_co_yield_statement = 334,
  sym_throw_statement = 335,
  sym_try_statement = 336,
  sym_catch_clause = 337,
  sym_co_await_expression = 338,
  sym_new_expression = 339,
  sym_new_declarator = 340,
  sym_delete_expression = 341,
  sym_lambda_expression = 342,
  sym_lambda_capture_specifier = 343,
  sym_lambda_default_capture = 344,
  sym_parameter_pack_expansion = 345,
  sym_type_parameter_pack_expansion = 346,
  sym_destructor_name = 347,
  sym_dependent_identifier = 348,
  sym_dependent_field_identifier = 349,
  sym_dependent_type_identifier = 350,
  sym__scope_resolution = 351,
  sym_qualified_field_identifier = 352,
  sym_qualified_identifier = 353,
  sym_qualified_type_identifier = 354,
  sym_qualified_operator_cast_identifier = 355,
  sym_operator_name = 356,
  sym_user_defined_literal = 357,
  aux_sym_translation_unit_repeat1 = 358,
  aux_sym_preproc_params_repeat1 = 359,
  aux_sym_preproc_if_in_field_declaration_list_repeat1 = 360,
  aux_sym_preproc_argument_list_repeat1 = 361,
  aux_sym_declaration_repeat1 = 362,
  aux_sym_type_definition_repeat1 = 363,
  aux_sym_type_definition_repeat2 = 364,
  aux_sym__declaration_specifiers_repeat1 = 365,
  aux_sym_attribute_declaration_repeat1 = 366,
  aux_sym_attributed_declarator_repeat1 = 367,
  aux_sym_pointer_declarator_repeat1 = 368,
  aux_sym_function_declarator_repeat1 = 369,
  aux_sym_function_declarator_repeat2 = 370,
  aux_sym_abstract_function_declarator_repeat1 = 371,
  aux_sym_sized_type_specifier_repeat1 = 372,
  aux_sym_enumerator_list_repeat1 = 373,
  aux_sym_field_declaration_repeat1 = 374,
  aux_sym_parameter_list_repeat1 = 375,
  aux_sym_case_statement_repeat1 = 376,
  aux_sym_argument_list_repeat1 = 377,
  aux_sym_initializer_list_repeat1 = 378,
  aux_sym_initializer_pair_repeat1 = 379,
  aux_sym_concatenated_string_repeat1 = 380,
  aux_sym_string_literal_repeat1 = 381,
  aux_sym_base_class_clause_repeat1 = 382,
  aux_sym_template_parameter_list_repeat1 = 383,
  aux_sym_field_initializer_list_repeat1 = 384,
  aux_sym_operator_cast_definition_repeat1 = 385,
  aux_sym_structured_binding_declarator_repeat1 = 386,
  aux_sym_throw_specifier_repeat1 = 387,
  aux_sym_template_argument_list_repeat1 = 388,
  aux_sym_try_statement_repeat1 = 389,
  aux_sym_lambda_capture_specifier_repeat1 = 390,
  alias_sym_field_identifier = 391,
  alias_sym_namespace_identifier = 392,
  alias_sym_statement_identifier = 393,
  alias_sym_type_identifier = 394,
};
