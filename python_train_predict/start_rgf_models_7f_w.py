from higgsml_functions import *

# define the directory to store everything temporary
temp_dir = 'temp'
myMakeDir(temp_dir)

# define the directory to store the models
save_dir = '../model_output'
myMakeDir(save_dir)

# load training data & process
train_dat,train_target,train_weights = loadAndProcessData("../data/training.csv")

# parameters for model
param = {}
param['algorithm'] = 'RGF'
param['reg_L2'] = 0.1
param['reg_sL2'] = 0.001
param['loss'] = 'Log'
param['test_interval'] = 1000
param['max_leaf_forest'] = 50000

# start training / cross validation
startTraining(train_dat,train_target,train_weights,'w',temp_dir,save_dir,param,predict_weights=True)
