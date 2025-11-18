set IM $::env(IMM)

project load ffn_dma64.ccs

flow package option set /SCVerify/INVOKE_ARGS $IM

flow run /SCVerify/launch_make ./scverify/Verify_concat_sim_ffn_v_msim.mk {} SIMTOOL=msim simgui


