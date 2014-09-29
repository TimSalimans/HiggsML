# -*- coding: utf-8 -*-
"""
Created on Sun Sep 28 20:48:25 2014

@author: Tim
"""
import subprocess
import os
import numpy as np
import pandas as pd

# RGF path
rgf_path = '../rgf1.2'    

def delDER(df):
    for col in df.columns.values:
        if col[:3]=="DER" and col!="DER_mass_MMC":
            df = df.drop(col, axis=1)            
    return df
    
def addFeatures(df):
    df['TIM_abs_eta_tau'] = np.abs(df.PRI_tau_eta)
    df['TIM_abs_eta_lep'] = np.abs(df.PRI_lep_eta)   
    df['TIM_abs_eta_jet1'] = np.abs(df.PRI_jet_leading_eta)   
    df['TIM_abs_eta_jet2'] = np.abs(df.PRI_jet_subleading_eta)       
    df['TIM_deltaeta_tau_lep'] = np.abs(df.PRI_tau_eta-df.PRI_lep_eta)
    df['TIM_deltaeta_tau_jet1'] = np.abs(df.PRI_tau_eta-df.PRI_jet_leading_eta)
    df['TIM_deltaeta_tau_jet2'] = np.abs(df.PRI_tau_eta-df.PRI_jet_subleading_eta)
    df['TIM_deltaeta_lep_jet1'] = np.abs(df.PRI_lep_eta-df.PRI_jet_leading_eta)
    df['TIM_deltaeta_lep_jet2'] = np.abs(df.PRI_lep_eta-df.PRI_jet_subleading_eta)
    df['TIM_deltaeta_jet_jet'] = np.abs(df.PRI_jet_leading_eta-df.PRI_jet_subleading_eta)

    df['TIM_prodeta_tau_lep'] = df.PRI_tau_eta*df.PRI_lep_eta
    df['TIM_prodeta_tau_jet1'] = df.PRI_tau_eta*df.PRI_jet_leading_eta
    df['TIM_prodeta_tau_jet2'] = df.PRI_tau_eta*df.PRI_jet_subleading_eta
    df['TIM_prodeta_lep_jet1'] = df.PRI_lep_eta*df.PRI_jet_leading_eta
    df['TIM_prodeta_lep_jet2'] = df.PRI_lep_eta*df.PRI_jet_subleading_eta
    df['TIM_prodeta_jet_jet'] = df.PRI_jet_leading_eta*df.PRI_jet_subleading_eta
    
    TIM_deltaphi_tau_lep = np.abs(df.PRI_tau_phi-df.PRI_lep_phi)
    TIM_deltaphi_tau_lep[TIM_deltaphi_tau_lep>np.pi] = 2*np.pi-TIM_deltaphi_tau_lep
    TIM_deltaphi_tau_jet1 = np.abs(df.PRI_tau_phi-df.PRI_jet_leading_phi)
    TIM_deltaphi_tau_jet1[TIM_deltaphi_tau_jet1>np.pi] = 2*np.pi-TIM_deltaphi_tau_jet1
    TIM_deltaphi_tau_jet2 = np.abs(df.PRI_tau_phi-df.PRI_jet_subleading_phi)
    TIM_deltaphi_tau_jet2[TIM_deltaphi_tau_jet2>np.pi] = 2*np.pi-TIM_deltaphi_tau_jet2
    TIM_deltaphi_lep_jet1 = np.abs(df.PRI_lep_phi-df.PRI_jet_leading_phi)
    TIM_deltaphi_lep_jet1[TIM_deltaphi_lep_jet1>np.pi] = 2*np.pi-TIM_deltaphi_lep_jet1
    TIM_deltaphi_lep_jet2 = np.abs(df.PRI_lep_phi-df.PRI_jet_subleading_phi)
    TIM_deltaphi_lep_jet2[TIM_deltaphi_lep_jet2>np.pi] = 2*np.pi-TIM_deltaphi_lep_jet2
    TIM_deltaphi_jet_jet = np.abs(df.PRI_jet_leading_phi-df.PRI_jet_subleading_phi)
    TIM_deltaphi_jet_jet[TIM_deltaphi_jet_jet>np.pi] = 2*np.pi-TIM_deltaphi_jet_jet
    
    df['TIM_deltar_tau_lep'] = np.sqrt(np.square(df.TIM_deltaeta_tau_lep)+np.square(TIM_deltaphi_tau_lep))
    df['TIM_deltar_tau_jet1'] = np.sqrt(np.square(df.TIM_deltaeta_tau_jet1)+np.square(TIM_deltaphi_tau_jet1))
    df['TIM_deltar_tau_jet2'] = np.sqrt(np.square(df.TIM_deltaeta_tau_jet2)+np.square(TIM_deltaphi_tau_jet2))
    df['TIM_deltar_lep_jet1'] = np.sqrt(np.square(df.TIM_deltaeta_lep_jet1)+np.square(TIM_deltaphi_lep_jet1))
    df['TIM_deltar_lep_jet2'] = np.sqrt(np.square(df.TIM_deltaeta_lep_jet2)+np.square(TIM_deltaphi_lep_jet2))
    df['TIM_deltar_jet_jet'] = np.sqrt(np.square(df.TIM_deltaeta_jet_jet)+np.square(TIM_deltaphi_jet_jet))
    
    #d = np.sign(np.pi - np.abs(df.PRI_tau_phi - df.PRI_lep_phi))
    d = df.PRI_tau_phi - df.PRI_lep_phi
    d = 1.0 - 2.0*((d>np.pi)|((d<0) & (d>-np.pi)))
    a = np.sin(df.PRI_met_phi-df.PRI_lep_phi)
    b = np.sin(df.PRI_tau_phi-df.PRI_met_phi)
    df['TIM_met_phi_centrality'] = d*(a+b)/np.sqrt(np.square(a)+np.square(b))
    
    df['TIM_lep_eta_centrality'] = np.exp(-4.0*np.square(df.PRI_lep_eta-(df.PRI_jet_leading_eta+df.PRI_jet_subleading_eta)/2)/np.square(df.PRI_jet_leading_eta-df.PRI_jet_subleading_eta))
    df['TIM_tau_eta_centrality'] = np.exp(-4.0*np.square(df.PRI_tau_eta-(df.PRI_jet_leading_eta+df.PRI_jet_subleading_eta)/2)/np.square(df.PRI_jet_leading_eta-df.PRI_jet_subleading_eta))   
    
    df['TIM_pt2_met_tau'] = (np.square(df.PRI_met*np.cos(df.PRI_met_phi) + df.PRI_tau_pt*np.cos(df.PRI_tau_phi))
        + np.square(df.PRI_met*np.sin(df.PRI_met_phi) + df.PRI_tau_pt*np.sin(df.PRI_tau_phi)))
    df['TIM_pt2_met_lep'] = (np.square(df.PRI_met*np.cos(df.PRI_met_phi) + df.PRI_lep_pt*np.cos(df.PRI_lep_phi))
        + np.square(df.PRI_met*np.sin(df.PRI_met_phi) + df.PRI_lep_pt*np.sin(df.PRI_lep_phi)))
    df['TIM_pt2_met_jet1'] = (np.square(df.PRI_met*np.cos(df.PRI_met_phi) + df.PRI_jet_leading_pt*np.cos(df.PRI_jet_leading_phi))
        + np.square(df.PRI_met*np.sin(df.PRI_met_phi) + df.PRI_jet_leading_pt*np.sin(df.PRI_jet_leading_phi)))
    df['TIM_pt2_met_jet2'] = (np.square(df.PRI_met*np.cos(df.PRI_met_phi) + df.PRI_jet_subleading_pt*np.cos(df.PRI_jet_subleading_phi))
        + np.square(df.PRI_met*np.sin(df.PRI_met_phi) + df.PRI_jet_subleading_pt*np.sin(df.PRI_jet_subleading_phi)))
    df['TIM_pt2_tau_lep'] = (np.square(df.PRI_tau_pt*np.cos(df.PRI_tau_phi) + df.PRI_lep_pt*np.cos(df.PRI_lep_phi))
        + np.square(df.PRI_tau_pt*np.sin(df.PRI_tau_phi) + df.PRI_lep_pt*np.sin(df.PRI_lep_phi)))
    df['TIM_pt2_tau_jet1'] = (np.square(df.PRI_tau_pt*np.cos(df.PRI_tau_phi) + df.PRI_jet_leading_pt*np.cos(df.PRI_jet_leading_phi))
        + np.square(df.PRI_tau_pt*np.sin(df.PRI_tau_phi) + df.PRI_jet_leading_pt*np.sin(df.PRI_jet_leading_phi)))
    df['TIM_pt2_tau_jet2'] = (np.square(df.PRI_tau_pt*np.cos(df.PRI_tau_phi) + df.PRI_jet_subleading_pt*np.cos(df.PRI_jet_subleading_phi))
        + np.square(df.PRI_tau_pt*np.sin(df.PRI_tau_phi) + df.PRI_jet_subleading_pt*np.sin(df.PRI_jet_subleading_phi)))    
    df['TIM_pt2_lep_jet1'] = (np.square(df.PRI_lep_pt*np.cos(df.PRI_lep_phi) + df.PRI_jet_leading_pt*np.cos(df.PRI_jet_leading_phi))
        + np.square(df.PRI_lep_pt*np.sin(df.PRI_lep_phi) + df.PRI_jet_leading_pt*np.sin(df.PRI_jet_leading_phi)))
    df['TIM_pt2_lep_jet2'] = (np.square(df.PRI_lep_pt*np.cos(df.PRI_lep_phi) + df.PRI_jet_subleading_pt*np.cos(df.PRI_jet_subleading_phi))
        + np.square(df.PRI_lep_pt*np.sin(df.PRI_lep_phi) + df.PRI_jet_subleading_pt*np.sin(df.PRI_jet_subleading_phi)))
    df['TIM_pt2_jet_jet'] = (np.square(df.PRI_jet_leading_pt*np.cos(df.PRI_jet_leading_phi) + df.PRI_jet_subleading_pt*np.cos(df.PRI_jet_subleading_phi))
        + np.square(df.PRI_jet_leading_pt*np.sin(df.PRI_jet_leading_phi) + df.PRI_jet_subleading_pt*np.sin(df.PRI_jet_subleading_phi)))    
    
    df['TIM_trans_mass_met_tau'] = np.sqrt(np.square(df.PRI_met+df.PRI_tau_pt)-df.TIM_pt2_met_tau)
    df['TIM_trans_mass_met_lep'] = np.sqrt(np.square(df.PRI_met+df.PRI_lep_pt)-df.TIM_pt2_met_lep)
    df['TIM_trans_mass_met_jet1'] = np.sqrt(np.square(df.PRI_met+df.PRI_jet_leading_pt)-df.TIM_pt2_met_jet1)
    df['TIM_trans_mass_met_jet2'] = np.sqrt(np.square(df.PRI_met+df.PRI_jet_subleading_pt)-df.TIM_pt2_met_jet2)
    df['TIM_trans_mass_tau_lep'] = np.sqrt(np.square(df.PRI_tau_pt+df.PRI_lep_pt)-df.TIM_pt2_tau_lep)
    df['TIM_trans_mass_tau_jet1'] = np.sqrt(np.square(df.PRI_tau_pt+df.PRI_jet_leading_pt)-df.TIM_pt2_tau_jet1)
    df['TIM_trans_mass_tau_jet2'] = np.sqrt(np.square(df.PRI_tau_pt+df.PRI_jet_subleading_pt)-df.TIM_pt2_tau_jet2)
    df['TIM_trans_mass_lep_jet1'] = np.sqrt(np.square(df.PRI_lep_pt+df.PRI_jet_leading_pt)-df.TIM_pt2_lep_jet1)
    df['TIM_trans_mass_lep_jet2'] = np.sqrt(np.square(df.PRI_lep_pt+df.PRI_jet_subleading_pt)-df.TIM_pt2_lep_jet2)
    df['TIM_trans_mass_jet_jet'] = np.sqrt(np.square(df.PRI_jet_leading_pt+df.PRI_jet_subleading_pt)-df.TIM_pt2_jet_jet)

    df['TIM_p2_tau_lep'] = df.TIM_pt2_tau_lep + np.square(df.PRI_tau_pt*np.sinh(df.PRI_tau_eta) + df.PRI_lep_pt*np.sinh(df.PRI_lep_eta))
    df['TIM_p2_tau_jet1'] = df.TIM_pt2_tau_jet1 + np.square(df.PRI_tau_pt*np.sinh(df.PRI_tau_eta) + df.PRI_jet_leading_pt*np.sinh(df.PRI_jet_leading_eta))
    df['TIM_p2_tau_jet2'] = df.TIM_pt2_tau_jet2 + np.square(df.PRI_tau_pt*np.sinh(df.PRI_tau_eta) + df.PRI_jet_subleading_pt*np.sinh(df.PRI_jet_subleading_eta))   
    df['TIM_p2_lep_jet1'] = df.TIM_pt2_lep_jet1 + np.square(df.PRI_lep_pt*np.sinh(df.PRI_lep_eta) + df.PRI_jet_leading_pt*np.sinh(df.PRI_jet_leading_eta))
    df['TIM_p2_lep_jet2'] = df.TIM_pt2_lep_jet2 + np.square(df.PRI_lep_pt*np.sinh(df.PRI_lep_eta) + df.PRI_jet_subleading_pt*np.sinh(df.PRI_jet_subleading_eta))
    df['TIM_p2_jet_jet'] = df.TIM_pt2_jet_jet + np.square(df.PRI_jet_leading_pt*np.sinh(df.PRI_jet_leading_eta) + df.PRI_jet_subleading_pt*np.sinh(df.PRI_jet_subleading_eta))

    df['E_tau'] = df.PRI_tau_pt*np.cosh(df.PRI_tau_eta)
    df['E_lep'] = df.PRI_lep_pt*np.cosh(df.PRI_lep_eta)
    df['E_jet1'] = df.PRI_jet_leading_pt*np.cosh(df.PRI_jet_leading_eta)
    df['E_jet2'] = df.PRI_jet_subleading_pt*np.cosh(df.PRI_jet_subleading_eta)

    df['TIM_mass_tau_lep'] = np.sqrt(np.square(df.E_tau+df.E_lep)-df.TIM_p2_tau_lep)
    df['TIM_mass_tau_jet1'] = np.sqrt(np.square(df.E_tau+df.E_jet1)-df.TIM_p2_tau_jet1)
    df['TIM_mass_tau_jet2'] = np.sqrt(np.square(df.E_tau+df.E_jet2)-df.TIM_p2_tau_jet2)
    df['TIM_mass_lep_jet1'] = np.sqrt(np.square(df.E_lep+df.E_jet1)-df.TIM_p2_lep_jet1)
    df['TIM_mass_lep_jet2'] = np.sqrt(np.square(df.E_lep+df.E_jet2)-df.TIM_p2_lep_jet2)
    df['TIM_mass_jet_jet'] = np.sqrt(np.square(df.E_jet1+df.E_jet2)-df.TIM_p2_jet_jet)

    sum_px = df.PRI_met*np.cos(df.PRI_met_phi) + df.PRI_tau_pt*np.cos(df.PRI_tau_phi) + df.PRI_lep_pt*np.cos(df.PRI_lep_phi)
    sum_py = df.PRI_met*np.sin(df.PRI_met_phi) + df.PRI_tau_pt*np.sin(df.PRI_tau_phi) + df.PRI_lep_pt*np.sin(df.PRI_lep_phi)
    df['TIM_pt_met_tau_lep'] = np.sqrt(np.square(sum_px) + np.square(sum_py))

    sum_px_2 = sum_px + (df.PRI_jet_leading_pt*np.cos(df.PRI_jet_leading_phi)).fillna(0.0)
    sum_py_2 = sum_py + (df.PRI_jet_leading_pt*np.sin(df.PRI_jet_leading_phi)).fillna(0.0)
    df['TIM_pt_met_tau_lep_jet1'] = np.sqrt(np.square(sum_px_2) + np.square(sum_py_2))

    sum_px_3 = sum_px_2 + (df.PRI_jet_subleading_pt*np.cos(df.PRI_jet_subleading_phi)).fillna(0.0)
    sum_py_3 = sum_py_2 + (df.PRI_jet_subleading_pt*np.sin(df.PRI_jet_subleading_phi)).fillna(0.0)
    df['TIM_pt_met_tau_lep_jet1_jet2'] = np.sqrt(np.square(sum_px_3) + np.square(sum_py_3))
    
    df['TIM_sum_pt_met_tau_lep'] = df.PRI_met + df.PRI_tau_pt + df.PRI_lep_pt
    df['TIM_sum_pt_met_tau_lep_jet1'] = df.TIM_sum_pt_met_tau_lep + df.PRI_jet_leading_pt.fillna(0.0)
    df['TIM_sum_pt_met_tau_lep_jet1_jet2'] = df.TIM_sum_pt_met_tau_lep_jet1 + df.PRI_jet_subleading_pt.fillna(0.0)
    df['TIM_sum_pt_met_tau_lep_jet_all'] = df.TIM_sum_pt_met_tau_lep_jet1 + df.PRI_jet_all_pt
    
    df['TIM_sum_pt'] = df.PRI_tau_pt + df.PRI_lep_pt + df.PRI_jet_all_pt

    df['TIM_pt_ratio_lep_tau'] = df.PRI_lep_pt/df.PRI_tau_pt

    return df
    
def delRawAngles(df):
    for col in df.columns.values:
        if col[len(col)-3:]=="phi" or col[len(col)-3:]=="eta":
            df = df.drop(col, axis=1)
    return df

def myMakeDir(dirname):
    if not os.path.exists(dirname):
        os.makedirs(dirname)
        
def start_RGF_train_predict(xtrain,ytrain,wtrain,xtest,modelname,temp_dir,save_dir,param):
    
    # handle data input
    xtrain = np.array(xtrain)
    xtest = np.array(xtest)
    ytrain = np.array(ytrain)
    
    # write data
    data_dir = temp_dir + '/' + modelname +'_data'
    myMakeDir(data_dir)
    np.savetxt(data_dir + '/trainx.txt', xtrain, delimiter=' ')
    np.savetxt(data_dir + '/trainy.txt', ytrain, delimiter=' ')
    if wtrain is not None:
        np.savetxt(data_dir + '/trainw.txt', wtrain.values, delimiter=' ')
    np.savetxt(data_dir + '/testx.txt', xtest, delimiter=' ')
    
    # write settings file
    output_dir = save_dir + '/' + modelname + '_output'
    with open (data_dir+'/'+modelname+'.inp', 'w') as fp:        
        fp.write('train_x_fn=' + data_dir + '/trainx.txt\n')
        fp.write('train_y_fn=' + data_dir + '/trainy.txt\n')
        if wtrain is not None:
            fp.write('train_w_fn=' + data_dir + '/trainw.txt\n')
        fp.write('test_x_fn=' + data_dir + '/testx.txt\n')        
        fp.write('model_fn_prefix=' + output_dir + '/m\n')
            
        for p in param.items():
            fp.write("%s=%s\n" % p)
            
        fp.write('SaveLastModelOnly\n')
        fp.write('Verbose')
        fp.close()
    
    # start RGF
    myMakeDir(output_dir)
    p = subprocess.Popen('perl ' + rgf_path + '/test/call_exe.pl ' + rgf_path + '/bin/rgf train_predict ' + data_dir + '/' + modelname,
                         shell=True, bufsize=1, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                     
    return p
    
def start_RGF_train(xtrain,ytrain,wtrain,modelname,temp_dir,save_dir,param):
    
    # handle data input
    xtrain = np.array(xtrain)
    ytrain = np.array(ytrain)
    
    # write data
    data_dir = temp_dir + '/' + modelname +'_data'
    myMakeDir(data_dir)
    np.savetxt(data_dir + '/trainx.txt', xtrain, delimiter=' ')
    np.savetxt(data_dir + '/trainy.txt', ytrain, delimiter=' ')
    if wtrain is not None:
        np.savetxt(data_dir + '/trainw.txt', wtrain.values, delimiter=' ')
    
    # write settings file
    output_dir = save_dir + '/' + modelname + '_output'
    with open (data_dir+'/'+modelname+'.inp', 'w') as fp:        
        fp.write('train_x_fn=' + data_dir + '/trainx.txt\n')
        fp.write('train_y_fn=' + data_dir + '/trainy.txt\n')
        if wtrain is not None:
            fp.write('train_w_fn=' + data_dir + '/trainw.txt\n')
        fp.write('model_fn_prefix=' + output_dir + '/m\n')
            
        for p in param.items():
            fp.write("%s=%s\n" % p)
            
        fp.write('Verbose')
        fp.close()
    
    # start RGF
    myMakeDir(output_dir)
    p = subprocess.Popen('perl ' + rgf_path + '/test/call_exe.pl ' + rgf_path + '/bin/rgf train ' + data_dir + '/' + modelname,
                         shell=True, bufsize=1, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                     
    return p
    
def loadAndProcessData(filename="training.csv"):
    # load data & process
    print("loading and processing data...")
    dat = pd.read_csv(filename, na_values=[-999])
    sel = (dat.DER_mass_MMC>0)
    dat = dat[sel]
    dat = delDER(dat)
    dat = addFeatures(dat)
    dat = delRawAngles(dat)
    dat = dat.fillna(-999.0)
    dat = dat.drop('EventId', axis=1)
    if 'Weight' in dat.columns:
        target = (dat.Label=='s').astype('float64')
        dat = dat.drop('Label', axis=1)
        weight = dat.Weight
        dat = dat.drop('Weight', axis=1)
        weight[target==1] = weight[target==1]/np.mean(weight[target==1])
        weight[target==0] = weight[target==0]/np.mean(weight[target==0])

        return [dat,target,weight]
    else:
        return dat
        
def startTraining(train_dat,train_target,train_weights,modelname,temp_dir,save_dir,param,predict_weights=False):
    n = len(train_dat)
    
    if predict_weights:
        train_dat['target'] = train_target
        train_dat = pd.concat([train_dat.copy(), train_dat.copy()], axis=0, ignore_index=True)
        train_target = pd.DataFrame(np.concatenate([np.ones(n), -np.ones(n)]))
        train_weights = pd.DataFrame(np.concatenate([train_weights.values, np.ones(n)]))
    
    # start full models
    print("starting RGF models and waiting for them to finish, this can take a while")
    procs = []
    procs.append(start_RGF_train(train_dat.values,2*train_target.values-1,train_weights,modelname+'_full',temp_dir,save_dir,param))

    # start cross-validation
    cv_ind = np.mod(np.arange(n),7)
    if predict_weights:
        train_dat = train_dat.copy()
        train_dat.target[:n] = 1.0
        train_dat.target[n:] = 0.0
        cv_ind = np.concatenate([cv_ind,cv_ind])

    procs=[]
    for i in xrange(7):
        if train_weights is not None:
            w_cv = train_weights[cv_ind!=i]
        else:
            w_cv = None
        procs.append(start_RGF_train_predict(train_dat[cv_ind!=i].values,2*train_target[cv_ind!=i].values-1.0,w_cv,train_dat[cv_ind==i].values,modelname+'_cv'+repr(i),temp_dir,save_dir,param))

    # display output & wait to finish
    print("will print RGF output to console")
    for p in procs:
        while p.poll() is None:
            output = p.stdout.readline()
            print output    
    print("all done")

def makePredictions(test_dat,temp_dir,save_dir):

    # write data
    data_dir = temp_dir + '/test_data'
    myMakeDir(data_dir)
    np.savetxt(data_dir + '/testx.txt', test_dat.values, delimiter=' ')

    # write data for 'weight' models
    test_dat_for_w_s = test_dat.copy()
    test_dat_for_w_s['target'] = 1.0
    test_dat_for_w_b = test_dat.copy()
    test_dat_for_w_b['target'] = 0.0
    test_dat_for_w_total = pd.concat([test_dat_for_w_s, test_dat_for_w_b], axis=0, ignore_index=True)
    np.savetxt(data_dir + '/testx_for_w.txt', test_dat_for_w_total.values, delimiter=' ')

    # make predictions for each model file in the model output folder
    for root, dirs, files in os.walk(save_dir):
        for file in files:
            if file.startswith("m-") and len(file)==4:
                with open (temp_dir+'/temp_pred.inp', 'w') as fp:
                    modelfile = os.path.join(root, file)
                    dirname = os.path.basename(os.path.normpath(root))
                    if dirname.startswith("w_"):
                        fp.write('test_x_fn=' + data_dir + '/testx_for_w.txt\n')
                    else:
                        fp.write('test_x_fn=' + data_dir + '/testx.txt\n')
                    fp.write('model_fn=' + modelfile + '\n')
                    fp.write('prediction_fn=' + modelfile + '.pred')
                    fp.close()
                    p = subprocess.Popen('perl ' + rgf_path + '/test/call_exe.pl ' + rgf_path + '/bin/rgf predict ' + temp_dir + '/temp_pred',
                         shell=True, bufsize=1, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
                    while p.poll() is None:
                        output = p.stdout.readline()
                        print output

