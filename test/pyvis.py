import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

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
parsed_data = parse_file('ft.txt')

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
ax.set_xlim(min(all_x) - 5, max(all_x) + 5)
ax.set_ylim(min(all_y) - 5, max(all_y) + 5)

# Создаем объект scatter с пустыми данными
scatter = ax.scatter([], [], s=30, c='blue', alpha=0.7)
time_text = ax.text(0.02, 0.95, '', transform=ax.transAxes, fontsize=12)

def init():
    """Инициализация анимации"""
    scatter.set_offsets(np.empty((0, 2)))
    time_text.set_text('')
    return scatter, time_text

def update(frame):
    """Обновление кадра анимации"""
    time, points = parsed_data[frame]
    
    # Подготавливаем данные для отображения
    coords = np.array([coord for (coord), _ in points])
    
    # Обновляем позиции частиц
    scatter.set_offsets(coords)
    
    # Обновляем текст с временем
    time_text.set_text(f'Время: {time:.6f} с\nЧастиц: {len(coords)}')
    
    return scatter, time_text

# Создаем анимацию
ani = FuncAnimation(
    fig, 
    update, 
    frames=len(parsed_data),
    init_func=init,
    blit=True,
    interval=200,  # 200 мс между кадрами
    repeat_delay=2000  # 2 секунды перед повторением
)

plt.tight_layout()
plt.show()

# Для сохранения анимации в файл (раскомментируйте при необходимости)
ani.save('particle_motion.gif', writer='pillow', fps=5)