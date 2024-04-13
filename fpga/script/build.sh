echo parameter 1 is the name of project tcl file
echo  copy firmdrive newst version from ftp

 vivado_path=/tools/Xilinx/Vivado/2019.1
 vivado=$vivado_path/bin/vivado
 echo $parentdir
 prj_path=$parentdir/prj

echo $vivado
echo $prj_path

if [ -z "$1" ]; then
  echo no input arguments
  export vivadoprj=z19
else
  export vivadoprj=$1
fi

prjscript=prj_$vivadoprj.tcl

if [ -f "$vivado" ]; then
  if [ -f "$prjscript" ]; then
    $vivado -mode batch -source build_project.tcl -notrace
  else
    echo $prjscript was not found.
  fi 
else
  echo .
  echo  ###############################
  echo  ### Failed to locate Vivado ###
  echo  ###############################
  echo .
  echo  This batch file "%~n0.bat" did not find Vivado installed in:
  echo .
  echo      %vivado%
  echo .
  echo  Fix the problem by doing one of the following:
  echo .
  echo   1. If you do not have this version of Vivado installed,
  echo      please install it or download the project sources from
  echo      a commit of the Git repository that was intended for
  echo      your version of Vivado.
  echo .
  echo   2. If Vivado is installed in a different location on your
  echo      PC, please modify the first line of this batch file 
  echo      to specify the correct location.
  echo .
  pause 
fi
