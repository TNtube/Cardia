; SPIR-V
; Version: 1.0
; Generator: Google Shaderc over Glslang; 11
; Bound: 40
; Schema: 0
OpCapability Shader
%1 = OpExtInstImport "GLSL.std.450"
OpMemoryModel Logical GLSL450
OpEntryPoint Vertex %main "main" %_ %gl_VertexIndex
OpSource GLSL 450
OpSourceExtension "GL_GOOGLE_cpp_style_line_directive"
OpSourceExtension "GL_GOOGLE_include_directive"
OpName %main "main"
OpName %positions "positions"
OpName %gl_PerVertex "gl_PerVertex"
OpMemberName %gl_PerVertex 0 "gl_Position"
OpMemberName %gl_PerVertex 1 "gl_PointSize"
OpMemberName %gl_PerVertex 2 "gl_ClipDistance"
OpMemberName %gl_PerVertex 3 "gl_CullDistance"
OpName %_ ""
OpName %gl_VertexIndex "gl_VertexIndex"
OpMemberDecorate %gl_PerVertex 0 BuiltIn Position
OpMemberDecorate %gl_PerVertex 1 BuiltIn PointSize
OpMemberDecorate %gl_PerVertex 2 BuiltIn ClipDistance
OpMemberDecorate %gl_PerVertex 3 BuiltIn CullDistance
OpDecorate %gl_PerVertex Block
OpDecorate %gl_VertexIndex BuiltIn VertexIndex
%void = OpTypeVoid
%3 = OpTypeFunction %void
%float = OpTypeFloat 32
%v2float = OpTypeVector %float 2
%uint = OpTypeInt 32 0
%uint_3 = OpConstant %uint 3
%_arr_v2float_uint_3 = OpTypeArray %v2float %uint_3
%_ptr_Private__arr_v2float_uint_3 = OpTypePointer Private %_arr_v2float_uint_3
%positions = OpVariable %_ptr_Private__arr_v2float_uint_3 Private
%float_0 = OpConstant %float 0
%float_n0_5 = OpConstant %float -0.5
%15 = OpConstantComposite %v2float %float_0 %float_n0_5
%float_0_5 = OpConstant %float 0.5
%17 = OpConstantComposite %v2float %float_0_5 %float_0_5
%18 = OpConstantComposite %v2float %float_n0_5 %float_0_5
%19 = OpConstantComposite %_arr_v2float_uint_3 %15 %17 %18
%v4float = OpTypeVector %float 4
%uint_1 = OpConstant %uint 1
%_arr_float_uint_1 = OpTypeArray %float %uint_1
%gl_PerVertex = OpTypeStruct %v4float %float %_arr_float_uint_1 %_arr_float_uint_1
%_ptr_Output_gl_PerVertex = OpTypePointer Output %gl_PerVertex
%_ = OpVariable %_ptr_Output_gl_PerVertex Output
%int = OpTypeInt 32 1
%int_0 = OpConstant %int 0
%_ptr_Input_int = OpTypePointer Input %int
%gl_VertexIndex = OpVariable %_ptr_Input_int Input
%_ptr_Private_v2float = OpTypePointer Private %v2float
%float_1 = OpConstant %float 1
%_ptr_Output_v4float = OpTypePointer Output %v4float
%main = OpFunction %void None %3
%5 = OpLabel
OpStore %positions %19
%30 = OpLoad %int %gl_VertexIndex
%32 = OpAccessChain %_ptr_Private_v2float %positions %30
%33 = OpLoad %v2float %32
%35 = OpCompositeExtract %float %33 0
%36 = OpCompositeExtract %float %33 1
%37 = OpCompositeConstruct %v4float %35 %36 %float_0 %float_1
%39 = OpAccessChain %_ptr_Output_v4float %_ %int_0
OpStore %39 %37
OpReturn
OpFunctionEnd
