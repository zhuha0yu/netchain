# Evaluation

## Start the program

Please follow steps in `src/README.md` to build and start the program.

## Start evaluation

Please enter `update` in host program to update the key and node assignment from csv file. Then enter `evaluate` to enter test environment.

## Evaluation performance

Please use `insert`, `write`, `read` and `thrp` to test the performance of NetChain. the log file will be created automatically in `evaluation/`.

## Evaluation robust

Please insert several keys first(if it is empty). Start the host program on one or several host and starts writing/reading. Manually down the switch links using `link sA sB down` in mininet CLI. Then enter the switch name, e.g. `s2` to Controller's program. The fail-over will perform.

After fail-over is finished, start writing/reading again an input anything on controller's program to start the evaluation of fail recovery.
