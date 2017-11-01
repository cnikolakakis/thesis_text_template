open_project canny
set_top canny
add_files canny.cpp
add_files -tb test.cpp
add_files -tb EasyBMP.cpp
open_solution "canny"
set_part {xc7z020clg484-1} -tool vivado
create_clock -period 10 -name default

#source "./canny/canny/directives.tcl"

puts "\n"
puts "*  Running csim_design to validate C code is functional"
csim_design
puts "\n"
puts "*  Running csynth_design to create core RTL"
csynth_design
puts "\n"
puts "*  Running cosim_design to verify RTL - using main() in test.cpp"
cosim_design
puts "\n"
puts "*  Running export_design to generate bus adapters and final IP package"
export_design
exit