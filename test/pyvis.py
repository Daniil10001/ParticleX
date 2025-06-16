import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

from matplotlib.scale import ScaleBase
from matplotlib.transforms import Transform

class DataSizeTransform(Transform):
    input_dims = output_dims = 2

    def __init__(self, ax, size):
        super().__init__()
        self.ax = ax
        self.size = size*2  # Диаметр в единицах данных

    def transform(self, values):
        # Рассчитываем площадь точек при текущем масштабе
        fig = self.ax.figure
        fig_width_inch = fig.get_figwidth()
        x_min, x_max = self.ax.get_xlim()
        units_per_inch = (x_max - x_min) / fig_width_inch
        diameter_points = self.size / units_per_inch * 72
        area = np.pi * (diameter_points / 2) ** 2
        return np.full_like(values, area)  # Одинаковый размер для всех

def parse_file(filename):
    data = []
    with open(filename, 'r') as file:
        for line in file:
            time_part, points_part = line.strip().split('|', 1)
            time = float(time_part)
            points_data = []
            records = points_part.split(';')
            
            for record in records:
                record = record.strip()
                if not record or '>' not in record:
                    continue
                
                coords_str, energy_str = record.rsplit('>', 1)
                coords = coords_str.replace('<', '').strip()
                try:
                    x1, x2 = map(float, coords.split(','))
                    energy = float(energy_str.strip())
                    points_data.append(((x1, x2), energy))
                except ValueError:
                    continue
            
            data.append((time, points_data))
    
    return data

# Считываем данные из файла
parsed_data = parse_file('res.txt')

# Создаем фигуру и оси
fig, ax = plt.subplots(figsize=(10, 8))
ax.set_title('Движение частиц во времени')
ax.set_xlabel('X1 координата')
ax.set_ylabel('X2 координата')
ax.grid(True, linestyle='--', alpha=0.7)

#print(parsed_data)

# Определяем границы графика
all_x = [coord[0] for time, points in parsed_data for (coord), _ in points]
all_y = [coord[1] for time, points in parsed_data for (coord), _ in points]
ax.set_xlim(min(all_x), max(all_x))
ax.set_ylim(min(all_y), max(all_y))

delim=[(0,500),(500,1000),(1000,1500),(1500,2000),(2000,2500),(2500,3000),(3000,3500),(3500,4000)]
colors=['blue','red']*4

delim=[(0,4000),(4000,8000)]
colors=['blue','red']

# Создаем объект scatter с пустыми данными
transform = DataSizeTransform(ax, size=0.01)
print(transform.transform(None))
scatters = [ax.scatter([], [], s=float(transform.transform(None)), c=colors[i], alpha=0.7) for i in range(len(colors))]
time_text = ax.text(0.02, 0.95, '', transform=ax.transAxes, fontsize=12)

def init():
    """Инициализация анимации"""
    for scatter in scatters:
        scatter.set_offsets(np.empty((0, 2)))
    time_text.set_text('')
    return *scatters, time_text

def update(frame):
    """Обновление кадра анимации"""
    time, points = parsed_data[frame]
    
    # Подготавливаем данные для отображения
    coords = np.array([coord for (coord), _ in points])
    
    # Обновляем позиции частиц
    for i,scatter in enumerate(scatters):
        scatter.set_offsets(coords[delim[i][0]:delim[i][1]])
    
    # Обновляем текст с временем
    time_text.set_text(f'Время: {time:.6f} с\nЧастиц: {len(coords)}')
    
    return *scatters, time_text

# Создаем анимацию
ani = FuncAnimation(
    fig, 
    update, 
    frames=len(parsed_data),
    init_func=init,
    blit=True,
    interval=100,  # 200 мс между кадрами
    repeat_delay=0  # 2 секунды перед повторением
)

plt.tight_layout()
plt.show()

# Для сохранения анимации в файл (раскомментируйте при необходимости)
#sani.save('particle_motion.gif', writer='pillow', fps=5)