opt_design -directive ExploreSequentialArea
place_design -directive SSI_SpreadLogic_low
phys_opt_design -critical_cell_opt
route_design -directive HigherDelayCost
