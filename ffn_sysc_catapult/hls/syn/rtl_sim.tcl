# set IM $::env(IMM)
puts $::env(SENTENCE)
# set ::env(SENTENCE) "boring"

project load ffn_dma64.ccs

# flow package option set /SCVerify/INVOKE_ARGS $IM

flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_ffn_v_msim.mk {} SIMTOOL=msim sim

