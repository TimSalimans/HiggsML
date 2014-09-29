% This performs non-negative linear blending of several RGF models for the
% HiggsML Challenge
% Run this in Matlab 2014 for support of data tables, or update the
% 'readtable' statement belows when using an older version
% Author: Tim Salimans

% load all the necessary data
model_dir = [pwd '/../model_output'];
data_dir = [pwd '/../data'];
traindat = readtable([data_dir '/training.csv']);
trainsel = traindat.DER_mass_MMC>0;
raw_weights = traindat.Weight(trainsel);
target = strcmp(traindat.Label(trainsel),'s');
train_weights = raw_weights;
train_weights(target) = train_weights(target)/mean(train_weights(target));
train_weights(~target) = train_weights(~target)/mean(train_weights(~target));

% load cross validated predictions
n = length(target);
cvind = mod(0:(n-1),7);
xw_cv = zeros(n,70);
xs_cv = zeros(n,70);
xwl_cv = zeros(n,70);
xw_exp_cv = zeros(n,70);
xs_exp_cv = zeros(n,70);
xwl_exp_cv = zeros(n,70);
for i=1:70
    if i<10
        nrs = ['0' int2str(i)];
    else
        nrs = int2str(i);
    end
    for j=0:6
        fn = [model_dir '/w_cv' int2str(j) '_output/m-' nrs '.pred'];
        if exist(fn,'file')
            preds = csvread(fn);
            np = length(preds)/2;
            xw_cv(cvind==j,i) = preds(1:np)-preds((np+1):end);
        end
        fn = [model_dir '/s_cv' int2str(j) '_output/m-' nrs '.pred'];
        if exist(fn,'file')
            xs_cv(cvind==j,i) = csvread(fn);
        end
        fn = [model_dir '/wl_cv' int2str(j) '_output/m-' nrs '.pred'];
        if exist(fn,'file')
            xwl_cv(cvind==j,i) = csvread(fn);
        end
        
        fn = [model_dir '/w_exp_cv' int2str(j) '_output/m-' nrs '.pred'];
        if exist(fn,'file')
            preds = csvread(fn);
            np = length(preds)/2;
            xw_exp_cv(cvind==j,i) = preds(1:np)-preds((np+1):end);
        end
        fn = [model_dir '/s_exp_cv' int2str(j) '_output/m-' nrs '.pred'];
        if exist(fn,'file')
            xs_exp_cv(cvind==j,i) = csvread(fn);
        end
        fn = [model_dir '/wl_exp_cv' int2str(j) '_output/m-' nrs '.pred'];
        if exist(fn,'file')
            xwl_exp_cv(cvind==j,i) = csvread(fn);
        end
    end
end

% load predictions on the test set
fn = [model_dir '/wl_exp_full_output/m-01.pred'];
preds = csvread(fn);
ntest = length(preds);
xw_test = zeros(ntest,70);
xs_test = zeros(ntest,70);
xwl_test = zeros(ntest,70);
xw_exp_test = zeros(ntest,70);
xs_exp_test = zeros(ntest,70);
xwl_exp_test = zeros(ntest,70);
for i=1:70
    if i<10
        nrs = ['0' int2str(i)];
    else
        nrs = int2str(i);
    end
    
    fn = [model_dir '/w_full_output/m-' nrs '.pred'];
    if exist(fn,'file')
        preds = csvread(fn);
        np = length(preds)/2;
        xw_test(:,i) = preds(1:np)-preds((np+1):end);
    end
    fn = [model_dir '/s_full_output/m-' nrs '.pred'];
    if exist(fn,'file')
        xs_test(:,i) = csvread(fn);
    end
    fn = [model_dir '/wl_full_output/m-' nrs '.pred'];
    if exist(fn,'file')
        xwl_test(:,i) = csvread(fn);
    end
    
    fn = [model_dir '/w_exp_full_output/m-' nrs '.pred'];
    if exist(fn,'file')
        preds = csvread(fn);
        np = length(preds)/2;
        xw_exp_test(:,i) = preds(1:np)-preds((np+1):end);
    end
    fn = [model_dir '/s_exp_full_output/m-' nrs '.pred'];
    if exist(fn,'file')
        xs_exp_test(:,i) = csvread(fn);
    end
    fn = [model_dir '/wl_exp_full_output/m-' nrs '.pred'];
    if exist(fn,'file')
        xwl_exp_test(:,i) = csvread(fn);
    end
end

% combine all the predictions, and only keep the complete ones
x_all = [ones(n,1) xw_cv xs_cv xwl_cv xw_exp_cv xs_exp_cv xwl_exp_cv];
x_full = [ones(ntest,1) xw_test xs_test xwl_test xw_exp_test xs_exp_test xwl_exp_test];
sel = ((sum(x_all==0)+sum(x_full==0))==0);
x_all = x_all(:,sel);
x_full = x_full(:,sel);
k = size(x_all,2);

% perform non-negative blending with exponential loss
obj = @(b)wexpreg(b,double(target),x_all,train_weights);
opt = optimset('GradObj','on','Hessian','on','Algorithm','trust-region-reflective','Display','iter','TolFun',1e-15);
b = fmincon(obj,zeros(k,1),[],[],[],[],[-Inf; zeros(k-1,1)],[],[],opt);

% define percentage cutoff
p = 0.175;

% write predictions
testdat = readtable([data_dir '/test.csv']);
testsel = testdat.DER_mass_MMC>0;
ftest = x_full*b;
rank_order = zeros(length(testsel),1);
rank_order(~testsel) = 1:(length(testsel)-length(ftest));
[~,ind] = sort(ftest);
rank_order(testsel) = ind + (length(testsel)-length(ftest));
pred_s = false(length(testsel),1);
pred_s(testsel) = ftest>=quantile(ftest,1-p);
pred_strings = repmat('b',length(pred_s),1);
pred_strings(pred_s) = 's';
to_predict = table(testdat.EventId,rank_order,cellstr(pred_strings),'VariableNames',{'EventId' 'RankOrder' 'Class'});
writetable(to_predict,['preds_' strrep(num2str(p),'.','_') '.csv']);    
