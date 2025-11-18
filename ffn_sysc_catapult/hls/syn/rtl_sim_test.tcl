puts $::env(SENTENCE)

set MODE_SIM $::env(RUN_ACCELERATED_SIM)
set arch $::env(arch)
set dma_width $::env(DMA_WIDTH)

puts $MODE_SIM

project load ffn_${arch}_sysc_catapult_dma${dma_width}.ccs


# set mdefines "-D$arch -DACCELERATED_SIM=$MODE_SIM"

# flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_conv_layer_sysc_catapult_v_msim.mk {} SIMTOOL=msim sim CXX_OPTS=-D$MODE_SIM
# export CXX_OPTS="-D$arch -DACCELERATED_SIM=$MODE_SIM"
# flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_ffn_v_msim.mk {USER_CXXFLAGS="-D$arch -DACCELERATED_SIM=$MODE_SIM"} SIMTOOL=msim sim
# flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_ffn_v_msim.mk {} SIMTOOL=msim sim CXX_OPTS="-D$arch -DACCELERATED_SIM=$MODE_SIM"
# flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_ffn_v_msim.mk "{SIMTOOL=msim}  {sim} {CXX_OPTS=-D$arch -DACCELERATED_SIM=$MODE_SIM}"
flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_ffn_sysc_catapult_v_msim.mk {} SIMTOOL=msim sim CXX_OPTS=-DACCELERATED_SIM=$MODE_SIM CXX_OPTS+=-D$arch