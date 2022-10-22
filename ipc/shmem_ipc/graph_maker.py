import pandas as pd
import matplotlib.pyplot as mp
 
import pandas as pd
import matplotlib.pyplot as mp
 
data = pd.read_csv('data/results.csv')

iterations = data['Iteration']
client_normal = data['Independent Client (normal)']
client_elevated = data['Independent Client (elevated)']
client_server_normal = data['Client + Server (normal)']
client_server_elevated = data['Client + Server (elevated)']

# form dataframe from data
df = pd.DataFrame(data, columns=[
    'Iteration',
    'Independent Client (normal)',
    'Independent Client (elevated)',
    'Client + Server (normal)',
    'Client + Server (elevated)'
])
 
# plot multiple columns such as population and year from dataframe
df.plot(x='Iteration', y=[
    'Independent Client (normal)',
    'Independent Client (elevated)',
    'Client + Server (normal)',
    'Client + Server (elevated)'
    ], kind="line", figsize=(10, 10))
 
# display plot
mp.show()