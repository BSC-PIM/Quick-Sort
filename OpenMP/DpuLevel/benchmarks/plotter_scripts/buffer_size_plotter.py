import matplotlib.pyplot as plt

with open('../results/buffer_size.csv', 'r') as f:
    data = f.read()

data = data.split('\n')

x = [0] * len(data)
y = [0] * len(data)

for i in range(len(data)):
    x[i] = 10 if i == 0 else x[i - 1] * 2
    y[i] = "{:.2f}".format(float((data[i].split(',')[0])))
    y[i] = float(y[i])

fig, ax = plt.subplots()
ax.plot(x, y, marker='o', linestyle='--', color='#e76680')  # Change the line color
ax.set_title('Time vs Buffer Size')
ax.set_ylabel('Time (ms)')
ax.set_xlabel('Buffer Size')

plt.show()

fig.savefig('../results/min_chunk_size.png')
