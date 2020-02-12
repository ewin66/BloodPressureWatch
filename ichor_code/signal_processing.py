import pywt
import pandas as pd
import numpy as np
import math
import matplotlib.pyplot as plt
from scipy.signal import argrelextrema
from scipy.signal import find_peaks, peak_prominences

def iswt(coefficients, wavelet):
    """
      M. G. Marino to complement pyWavelets' swt.
      Input parameters:

        coefficients
          approx and detail coefficients, arranged in level value
          exactly as output from swt:
          e.g. [(cA1, cD1), (cA2, cD2), ..., (cAn, cDn)]

        wavelet
          Either the name of a wavelet or a Wavelet object

    """
    output = coefficients[0][0].copy() # Avoid modification of input data

    #num_levels, equivalent to the decomposition level, n
    num_levels = len(coefficients)
    for j in range(num_levels,0,-1):
        step_size = int(math.pow(2, j-1))
        last_index = step_size
        _, cD = coefficients[num_levels - j]
        for first in range(last_index): # 0 to last_index - 1

            # Getting the indices that we will transform
            indices = np.arange(first, len(cD), step_size)

            # select the even indices
            even_indices = indices[0::2]
            # select the odd indices
            odd_indices = indices[1::2]

            # perform the inverse dwt on the selected indices,
            # making sure to use periodic boundary conditions
            x1 = pywt.idwt(output[even_indices], cD[even_indices], wavelet, 'per')
            x2 = pywt.idwt(output[odd_indices], cD[odd_indices], wavelet, 'per')

            # perform a circular shift right
            x2 = np.roll(x2, 1)

            # average and insert into the correct indices
            output[indices] = (x1 + x2)/2.

    return output

def apply_threshold(output, scaler = 1., input=None):

    """
        output
          approx and detail coefficients, arranged in level value
          exactly as output from swt:
          e.g. [(cA1, cD1), (cA2, cD2), ..., (cAn, cDn)]
        scaler
          float to allow runtime tuning of thresholding
        input
          vector with length len(output).  If not None, these values are used for thresholding
          if None, then the vector applies a calculation to estimate the proper thresholding
          given this waveform.
    """

    for j in range(len(output)):
        cA, cD = output[j]
        if input is None:
            dev = np.median(np.abs(cD - np.median(cD)))/0.6745
            thresh = math.sqrt(2*math.log(len(cD)))*dev*scaler
        else:
            threshA = scaler*input[j][0]
            threshD = scaler*input[j][1]
        cA = pywt.threshold(cA, threshA, 'soft')
        cD = pywt.threshold(cD, threshD, 'soft')
        output[j] = (cA, cD)

def measure_threshold(output, scaler = 1.):
    """
        output
          approx and detail coefficients, arranged in level value
          exactly as output from swt:
          e.g. [(cA1, cD1), (cA2, cD2), ..., (cAn, cDn)]
        scaler
          float to allow runtime tuning of thresholding

        returns vector of length len(output) with treshold values

    """
    measure = []
    for j in range(len(output)):
        cA, cD = output[j]
        devA = np.median(np.abs(cA - np.median(cA)))/0.6745
        devD = np.median(np.abs(cD - np.median(cD)))/0.6745
        threshA = math.sqrt(2*math.log(len(cA)))*devA*scaler
        threshD = math.sqrt(2*math.log(len(cD)))*devD*scaler
        measure.append((threshA, threshD))
    return measure

def smooth(x,window_len=11,window='hanning'):
    """smooth the data using a window with requested size.

    This method is based on the convolution of a scaled window with the signal.
    The signal is prepared by introducing reflected copies of the signal
    (with the window size) in both ends so that transient parts are minimized
    in the begining and end part of the output signal.

    input:
        x: the input signal
        window_len: the dimension of the smoothing window; should be an odd integer
        window: the type of window from 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'
            flat window will produce a moving average smoothing.

    output:
        the smoothed signal

    example:

    t=linspace(-2,2,0.1)
    x=sin(t)+randn(len(t))*0.1
    y=smooth(x)

    see also:

    numpy.hanning, numpy.hamming, numpy.bartlett, numpy.blackman, numpy.convolve
    scipy.signal.lfilter

    TODO: the window parameter could be the window itself if an array instead of a string
    NOTE: length(output) != length(input), to correct this: return y[(window_len/2-1):-(window_len/2)] instead of just y.
    """

    if x.ndim != 1:
        raise ValueError("smooth only accepts 1 dimension arrays.")

    if x.size < window_len:
        raise ValueError("Input vector needs to be bigger than window size.")


    if window_len<3:
        return x


    if not window in ['flat', 'hanning', 'hamming', 'bartlett', 'blackman']:
        raise ValueError("Window is on of 'flat', 'hanning', 'hamming', 'bartlett', 'blackman'")


    s=np.r_[x[window_len-1:0:-1],x,x[-2:-window_len-1:-1]]
    #print(len(s))
    if window == 'flat': #moving average
        w=np.ones(window_len,'d')
    else:
        w=eval('np.'+window+'(window_len)')

    y=np.convolve(w/w.sum(),s,mode='valid')
    return y

def denoise(x):
    output = pywt.swt(x, 'db2', level=2)
    threshold_list = measure_threshold(output)
    apply_threshold(output, 2.1, threshold_list)
    x_hat = iswt(output, 'db2')
    x_hat = smooth(x_hat)

    # cA, cD = pywt.dwt(x, 'db4')
    #
    #
    # # print(cA)
    # # print(cD)
    #
    # # print(cA)
    # x_hat = pywt.idwt(None, cD, 'db4')

    return x_hat

def peak_finder(x_hat):
    peaks, _ = find_peaks(x_hat)
    prominences = peak_prominences(x_hat, peaks)[0]

    threshold = np.sort(prominences)[::-1][2]
    peaks, _ = find_peaks(x_hat, prominence=(threshold, None))
    prominences = peak_prominences(x_hat, peaks)[0]
    contour_heights = x_hat[peaks] - prominences
    plt.vlines(x=peaks, ymin=contour_heights, ymax=x_hat[peaks])
    return peaks

def load_data():
    x = np.loadtxt('./Data/2_1.txt')
    # print(array)
    # plt.plot(array)
    return x
if __name__ == '__main__':
    x = load_data()
    x_hat = denoise(x)
    plt.plot(np.arange(x.shape[0]), x, np.arange(x_hat.shape[0]), x_hat)
    plt.legend(['original', 'reconstructed'])
    peaks = peak_finder(x_hat)
    zipped = zip(peaks[:-1], peaks[1:])
    ptt = np.array([(b-a)/600.0 for a, b in zipped])
    print(ptt)
    plt.title('PPG Waveform')
    plt.xlabel('N')
    plt.ylabel('PPG Value')
    plt.show()
