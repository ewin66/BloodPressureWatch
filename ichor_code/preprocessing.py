import pandas as pd
import numpy as np
import os
import matplotlib.pyplot as plt
# import seaborn as sns
plt.style.use('seaborn-white')

# Check that the directory is correct
dir = "../Watch Data/Luv Test New"
files = os.listdir(dir)


temp_df = pd.read_csv('{}/{}'.format(dir, files[0]))

# message = np.array(df['Message'][0].split(' '))[:-1]
# message = message.astype('long')
# print(message)
def get_data(msg_idx, raw_data=temp_df['Message']):
    arr = np.fromstring(raw_data[msg_idx], dtype='int64', sep='\n')
    ppg = pd.Series(arr[::2], name='ppg')
    time = pd.Series(arr[1::2], name='time')

    return time, ppg

def get_total_time_series(raw_data=temp_df['Message']):
    df = pd.DataFrame()
    for i in range(raw_data.shape[0]):
        time, ppg = get_data(i, raw_data=raw_data)
        new_data = pd.concat([time, ppg], axis=1)
        df = pd.concat([df, new_data], axis=0)
    return df

df = get_total_time_series()
# df = df.iloc[300:]
# _, ppg = get_data(12)
print(df.shape)
# print(df['ppg'].head(100))
plt.plot(range(df.shape[0]), df['ppg'], 'r')
plt.xlabel('n')
plt.title('Mayank PPG Waveform')
plt.savefig('{}.png'.format(dir.split('/')[2]))

# df.to_csv('Light Data.csv')
plt.show()
# df['time'] = pd.to_datetime(df['time'])
# df.set_index(df['time'], inplace=True)
# print(df.head())
# print(ppg)
# print(time)


# print(df['Message'][0])
