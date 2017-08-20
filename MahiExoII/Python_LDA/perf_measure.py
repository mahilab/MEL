#import numpy as np

def perf_measure(y_actual, y_hat):
    TP = 0
    FP = 0
    TN = 0
    FN = 0

    for c in range(1, 16):
      for i in range(len(y_hat)): 
         if y_actual[i]==y_hat[i]==c:
            TP += 1
      for i in range(len(y_hat)): 
          if y_actual[i]!=c and y_hat[i]==c:
            FP += 1
      for i in range(len(y_hat)): 
          if y_actual[i]!=c and y_hat[i]!=c:
             TN += 1
      for i in range(len(y_hat)): 
         if y_actual[i]==c and y_hat[i]!=c:
            FN += 1

    return(TP, TN, FP, FN)