from higgsml_functions import *

# define the directory to store everything temporary
temp_dir = 'temp'
myMakeDir(temp_dir)

# define the directory to find the models in
save_dir = '../model_output'

# load test data & process
test_dat = loadAndProcessData("../data/test.csv")

# make predictions for each model in 'save_dir'
makePredictions(test_dat,temp_dir,save_dir)
