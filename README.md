With this project, I hope to be able to classify different forms of motion (walking, running, jumping, etc.) using the Intel Edison and the 9DOF. In order to be able to classify these forms of motion, I'm going to make use of the FANN (Fast Artificial Neural Network) library.

The method of classifcation goes as follows:

1. Obtain accelerometer and gyroscope readings in order to analyze the waveforms of a particular form of motion.
2. Create a classifier in order to be able to train our neural network to be able to differentiate between different activities.
3. Test the neural network.
