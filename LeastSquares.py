def make_matr(ar, k):
    A = (np.array([np.ones(ar.shape)])).T
    #print(A)
    cur = np.array([ar]).T
    for i in range(0, k):
        A = np.concatenate((A, cur), axis=1)
        for j in range(0, ar.shape[0]):
            cur[j] = cur[j] * ar[j]
    return A
            
An_all = []
for k in range(1, 11):
    A = make_matr(X_train, k)
    #print(A)
    Z = np.array([Y_train])
    AT = np.transpose(A)
    An = (sla.inv(AT.dot(A)).dot(AT)).dot(Z.T)
    An_all.append(An)
    for i in range(0, k + 1):
        if(i == 0):
            print(An[i][0], sep="", end = "")
        else:
            print(' + ', An[i][0], 'x^', i, sep="", end="")
    print()
    
print("Mistake in train")
for k in range(0, 10):
    su = 0
    for x in range(X_train.shape[0]):
        cu = 1
        an = 0
        for i in range(k + 2):
            an = an + cu * An_all[k][i]
            cu = cu * X_train[x]
        su = su + ((an - Y_train[x])**2)
    su = su / (X_train.shape[0])
    print(su)
print("Mistake in test")
for k in range(0, 10):
    su = 0
    for x in range(X_test.shape[0]):
        cu = 1
        an = 0
        for i in range(k + 2):
            an = an + cu * An_all[k][i]
            cu = cu * X_test[x]
        su = su + ((an - Y_test[x])**2)
    su = su / (X_test.shape[0])
    print(su)
