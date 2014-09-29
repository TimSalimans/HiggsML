% linear classification with exponential loss
function [nllh,grad,hess] = wexpreg(b,y,x,w)

f = x*b;
pos = (w.*y).*exp(-f);
neg = (w.*(1-y)).*exp(f);
nllh = sum(pos)+sum(neg);

if nargout>=2
    df = neg-pos;
    grad = x'*df;    
end

if nargout==3
    dh = pos+neg;
    hess = x'*bsxfun(@times,x,dh);
end