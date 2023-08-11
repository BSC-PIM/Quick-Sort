import matplotlib.pyplot as plt
import numpy as np

with open('../results/min_chunk_size.csv', 'r') as f:
    data = f.read()

data = data.split('\n')

x = [0] * len(data)
y = [0] * len(data)

for i in range(len(data)):
    x[i] = 1 << i
    y[i] = "{:.2f}".format(float((data[i].split(',')[0])))
    y[i] = float(y[i])

fig, ax = plt.subplots()
ax.plot(x, y, marker='o', linestyle='-', color='b')
ax.set_title('Time vs Minimum Chunk Size (n = 1,000,000)')
ax.set_ylabel('Time (ms)')
ax.set_xlabel('Minimum Chunk Size')
plt.yscale('log')

# Find the index of the minimum y value
min_y_index = np.argmin(y)
min_x = x[min_y_index]
min_y = y[min_y_index]

# Annotate the minimum point with an arrow and text
ax.annotate(f'Min: {min_y:.2f} ms at {min_x}',
            xy=(min_x, min_y),
            xytext=(min_x * 2.2, min_y * 2),  # Adjust the arrow text position
            arrowprops=dict(facecolor='blue', arrowstyle='->', linewidth=2),  # Make the arrow bolder
            ha='center', fontsize=12)

plt.show()

fig.savefig('../results/min_chunk_size.png')
