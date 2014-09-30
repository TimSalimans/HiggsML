HiggsML
=======

This is the code for my second place finish in Kaggle's [HiggsML challenge](http://www.kaggle.com/c/higgs-boson). It is a blend of a large number of boosted decision tree ensembles constructed using [Regularized Greedy Forest](http://arxiv.org/pdf/1109.0887.pdf). I mostly used the features provided by the contest organizers, but I also added some additional ones and removed some that did not seem useful (the phi features). The blend consists of both models that estimate the weighted signal to background ratio directly, as well as models that estimate the unweighted outcomes and the weights separately. More details below. 

**Reproducing my result can be done by taking the following steps:**

#### 1. Training

1. Place the training file "training.csv" in the data directory.
2. Run the following scripts in the python_train_predict directory:
  * start_rgf_models_7_f_s.py
  * start_rgf_models_7_f_s_exp.py
  * start_rgf_models_7_f_w.py
  * start_rgf_models_7_f_w_exp.py
  * start_rgf_models_7_f_wl.py
  * start_rgf_models_7_f_wl_exp.py

The scripts run 7 fold cross validation and train a model on the complete training set. Running these takes a lot of memory and CPU's. I recommend to run each script separately on a machine with 8+ cores and 64+ GB of memory, wait 2 days, and then to collect the results. I myself used the *m2.4xlarge* instances on [Amazon EC2](http://aws.amazon.com/ec2/). You can also download the results from my [Google drive](https://drive.google.com/file/d/0B4Zly9eEgwFsbUx5cm15UHpJZTg/edit?usp=sharing).

#### 2. Predicting the test set
  
1. Place the test file "test.csv" in the data directory.
2. Run "make_predictions.py" in the python_train_predict directory to run the trained RGF models on the test set. This is relatively lightweight and can be done on a single machine if you have some patience. (the predictions are also included in my [Google drive](https://drive.google.com/file/d/0B4Zly9eEgwFsbUx5cm15UHpJZTg/edit?usp=sharing))

#### 3. Blending

1. Make sure "training.csv" and "test.csv" are both in the data directory and you have all the results from the 2 steps above.
2. Run the MATLAB script "do_blend.m" in the matlab_blend directory. This produces my final prediction file and writes it to the matlab_blend directory.

## Dependencies
* The scripts each train 8 models in parallel. This requires 64GB+ of memory.
* The [RGF package](http://stat.rutgers.edu/home/tzhang/software/rgf/) was built using 64bit Linux
* Perl should be installed and present on the path
* The Python packages pandas and numpy should be installed. These are included with any of the major Python distributions (Anaconda, EPD, Canopy, etc)
* The blending part requires MATLAB 2014

## Modeling

### Feature engineering
The challenge organizers already did a great job of providing features for this competition based on what physicists are currently using. I made the following changes/additions to the provided features

* Calculate the features with respect to new combinations of (pseudo)particles. For example, features like the *transverse mass* can be defined with respect to any combination of the particles in the detector, not just those for which it was calculated in the provided features.
* Transform the *eta* and *tau* features to account for the symmetries outlined on the [competition forum](http://www.kaggle.com/c/higgs-boson/forums/t/9576/reducing-the-feature-space).


### Prediction target
The problem being solved in this competition is a relatively standard classification problem where *collision events* (rows of data) are classified as either *signal* 's' or *background* 'b'. The accuracy of your solution was judged based on the *AMS metric*, which basically requires you to:

1. Build a model to estimate the ratio R = E(weight * I(label='s') | x) / E(weight * I(label='b') | x)
2. Choose a cutoff percentage and predict 's' for those rows where this estimated ratio is highest.

Here *I()* is a binary indicator function, *weight* is the weight for a row of data (not given for the test set), and *x* is the detector data that we base our prediction on.

Rather than focusing on the nonstandard AMS metric and empirical error minimization, my approach was to try and maximize statistical efficiency. The ratio given above can be decomposed as R = R1 * R2, where

R1 = P( label='s' | x) / P( label='b' | x)

R2 = E( weight | label='s', x ) / E( weight | label='b', x)

I figured it would be more statistically efficient to estimate the ratios R1 and R2 from the data separately and then to combine them rather than estimating R directly, as the effective sample size for estimating R1 and R2 is larger than for estimating R. Estimating the first ratio can be done using any standard classifier with logistic or exponential loss. The second ratio can be estimated this way as well with a little hacking:

* add the "target" (label 's' or 'b') to the training feature set
* copy the training data set twice
* set the prediction target for the first copy equal to +1, and for the second half to -1
* set the weights of the first half equal to the given weights, and for the second half equal to 1.0
* do the same for the test set, setting the (unobserved) label equal to 's' for the first half and 'b' for the second half
* get the predictions for both parts and divide them (subtract the log-odds) to get your ratio

Empirically I found that this *indirect approach* performed about equally well as the direct approach. Combining both in a blend gave the best results.

### RGF models
To build all of the submodels I used the excellent *Regularized Greedy Forest* package. This package builds ensembles of decision trees, similar to other boosting packages such as R's GBM or Python's XGBoost. The performance seemed similar, or slightly better, than XGboost. For this competition it was important to use an algorithm that is able to search through the model space effectively. RGF and XGboost seem to do this better than R's GBM implementation. I think *good model search* also explains the success of the neural network models used by the number 1 and 3 finishers in this competition.

RGF requires you to set two parameters: a regularization constant and the number of leaves in the model. I set the regularization constant to the largest value that gave an acceptable training time. I then used to model to generate predictions every time 1000 leaves were added to the model, and let the blending sort out which of these predictions to use.

### Blending
The blending model is a standard generalized linear model with exponential loss, estimated on the 7-fold cross validated predictions of the sub models. In order to make the results more stable and robust the coefficients in this model are restricted to be non-negative.

### Other things I tried that didn't work

* Explicitly regularizing the model to be invariant to the measurement error generated by the detector
* Using the test data as unlabeled data to guide the model search

### Speeding things up / reducing memory requirements
The model settings were chosen to give optimal predictive results in an amount of time that is acceptable when you can get some cheap Amazon EC2 instances using spot pricing. If you want to run this on your laptop it would probably be a good idea to reduce the number of leaves in the RGF models (and increase the regularization). Also you may not be able to run 7 fold cross validation and final model estimation in parallel if you have <60GB of memory on your machine.

## References
* http://www.kaggle.com/c/higgs-boson/data "Higgs Boson Machine Learning Challenge"
* http://arxiv.org/pdf/1109.0887.pdf Rie Johnson and Tong Zhang. "Learning nonlinear functions using regularized greedy forest", *IEEE Transactions on Pattern Analysis and Machine Intelligence*, 36(5):942-954, May 2014.
* http://stat.rutgers.edu/home/tzhang/software/rgf/ "Regularized Greedy Forest (RGF) in C++"

