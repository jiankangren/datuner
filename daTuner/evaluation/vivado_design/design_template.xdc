
####################################################################################
# Generated by Vivado 2016.1 built on 'Fri Apr  8 15:45:23 MDT 2016' by 'xbuild'
# Command Used: write_xdc design.xdc
####################################################################################


####################################################################################
# Constraints from file : 'ericsson_inlk_rx_k7_7s_spr.xdc'
####################################################################################


####################################################################################
# Generated by PlanAhead 13.4.OR0 built on 'Tue Aug 30 14:52:15 MDT 2011' by 'hdbuild'
####################################################################################


####################################################################################
# Constraints from file : 'inlk_rx.ucf'
####################################################################################


# All timing constraint translations are rough conversions, intended to act as a template for further manual refinement. The translations should not be expected to produce semantically identical results to the original ucf. Each xdc timing constraint must be manually inspected and verified to ensure it captures the desired intent

# /home/cdunlap/work/ericsson/congestion_test/project_1/project_1.srcs/constrs_1/new/inlk_rx.ucf:1
create_clock -period PERIOD_HOLDER -name clk [get_ports clk]



# User Generated miscellaneous constraints 

set_property BITSTREAM.GENERAL.UNCONSTRAINEDPINS Allow [current_design]

# Vivado Generated miscellaneous constraints 

#revert back to original instance
current_instance -quiet
