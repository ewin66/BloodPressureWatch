import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
#import tensorflow
from keras.models import load_model
from keras.models import Sequential
from keras.layers import LSTM, Dense, Dropout, Activation, TimeDistributed, Bidirectional
# from keras.layers import LSTM, TimeDistributed
from keras.optimizers import SGD
from keras.callbacks.callbacks import ModelCheckpoint, EarlyStopping

from sklearn.model_selection import train_test_split

df = pd.read_csv('./Data/allbp.csv')

X = df[['{}'.format(i) for i in range(1, 2101, 10)]].copy().values
n, p = X.shape
X = X.reshape(n, 1, p)
# print(X.shape)
y_sys = df['sys_mmgh'].copy().values
y_sys = y_sys.reshape(-1, 1, 1)
y_dia = df['dia_mmgh'].copy().values
y_dia = y_dia.reshape(-1, 1, 1)
split_idx = int(np.ceil(0.85*X.shape[0]))

X_train = X[:split_idx]
X_test = X[split_idx:]
y_train = y_sys[:split_idx]
y_test = y_sys[split_idx:]

print(X_test.shape)
# , X_test, y_train, y_test = train_test_split(X, y_sys, random_state=42)
# print(y_sys.shape)



def train_model(model_name, month, day, bp_type, epochs, X_train, y_train, X_test, y_test):

    model = Sequential()
    # model.add(Dense(units=100, input_shape=(1, X_train.shape[2])))
    # model.add(Dense(units=64))
    # model.add(Dense(units=32))
    # model.add(Bidirectional(LSTM(units=50, input_shape=(1, X_train.shape[2]), return_sequences=True)))
    model.add(LSTM(units=128, dropout=0.5, recurrent_dropout=0.5))
    model.add(Dense(128))
    model.add(Activation('relu'))
    model.add(Dropout(0.5))
    model.add(Dense(1))

    # sgd = SGD(learning_rate=0.00002, momentum=0.9, nesterov=True)
    model.compile(loss='mse', optimizer='adam', metrics=['mean_absolute_error'])


    earlyStopping = EarlyStopping(monitor='val_mean_absolute_error', patience=15, verbose=0, mode='min')
    mcp_save = ModelCheckpoint('{}_{}_{}.h5'.format(model_name, month, day), save_best_only=True, monitor='val_mean_absolute_error', mode='min')
    history = model.fit(X_train, y_train, epochs=epochs, batch_size=32, validation_data=(X_test, y_test), callbacks=[mcp_save, earlyStopping])


    # Save model to HDF5 for later use
    # model.save("model.h5")
    print("Saved model to disk")


    plt.plot(history.history['mean_absolute_error'])
    plt.plot(history.history['val_mean_absolute_error'])
    plt.title('{} Blood Pressure Model Error (RNN)'.format(bp_type))
    plt.ylabel('Mean Absolute Error (mmHg)')
    plt.xlabel('Epoch')
    plt.legend(['Training', 'Testing'], loc='best')
    plt.savefig('./Models/{}_{}_{}.png'.format(model_name, month, day), dpi=600)
    print('Saved graph to disk')

    plt.close('all')
    del model
    model = load_model('./Models/{}_{}_{}.h5'.format(model_name, month, day))
    plt.plot(y_test.reshape(-1, 1))
    # X_train = X_train.reshape(-1, 2100, 1)
    plt.plot(model.predict(X_test).reshape(-1, 1))
    plt.title('{} Blood Pressure'.format(bp_type))
    plt.xlabel('Data point')
    plt.ylabel('{} BP (mmHg)'.format(bp_type))
    plt.legend(['Ground Truth', 'Prediction'])
    plt.show()
    return model, history



model, history = train_model('play_sys', 11, 21, 'Systolic', 300, X_train, y_train, X_test, y_test)

# sys_model = load_model('play_sys_11_21.h5')
# # dia_model = load_model('model_dia_11_13.h5')
# #
# # print(sys_model.predict(X_test))
# plt.plot(y_test)
# plt.plot(sys_model.predict(X_test))
# plt.title('Predictions vs. Actual')
# plt.xlabel('Data point')
# plt.ylabel('Systolic BP (mmHg)')
# plt.legend(['Ground Truth', 'Actual Systolic'])
# plt.show()
