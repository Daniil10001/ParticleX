import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from collections import defaultdict

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
                    # Нам нужна только энергия для гистограммы
                    energy = float(energy_str.strip())
                    points_data.append(energy)
                except ValueError:
                    continue
            
            data.append((time, points_data))
    
    return data

# Считываем данные из файла
parsed_data = parse_file('res.txt')

# Создаем фигуру и оси
fig, ax = plt.subplots(figsize=(10, 6))
ax.set_title('Распределение энергии частиц по времени')
ax.set_xlabel('Энергия')
ax.set_ylabel('Количество частиц')
ax.grid(True, linestyle='--', alpha=0.7)

# Собираем все значения энергии для определения границ гистограммы
all_energies = []
for time, energies in parsed_data:
    all_energies.extend(energies)

# Создаем бины для гистограммы
min_energy = min(all_energies)
max_energy = max(all_energies)
num_bins = 20
bin_width = (max_energy - min_energy) / num_bins
bins = np.linspace(min_energy - bin_width, max_energy + bin_width, num_bins + 1)

# Предварительно вычисляем гистограммы для каждого момента времени
histograms = []
for time, energies in parsed_data:
    hist, bin_edges = np.histogram(energies, bins=bins)
    histograms.append(hist)

# Находим максимальное значение по оси Y для фиксированного масштаба
max_count = max(max(hist) for hist in histograms) + 1
ax.set_ylim(0, max_count)

# Центры бинов для отрисовки
bin_centers = (bins[:-1] + bins[1:]) / 2
width = bin_width * 0.8

# Создаем объект столбцов гистограммы
bars = ax.bar(bin_centers, histograms[0], width=width, 
              align='center', alpha=0.7, color='blue')

# Текстовые элементы
time_text = ax.text(0.02, 0.95, '', transform=ax.transAxes, fontsize=12)
count_text = ax.text(0.02, 0.90, '', transform=ax.transAxes, fontsize=10)

def init():
    """Инициализация анимации"""
    for bar in bars:
        bar.set_height(0)
    time_text.set_text('')
    count_text.set_text('')
    return bars, time_text, count_text

def update(frame):
    """Обновление кадра анимации"""
    time, energies = parsed_data[frame]
    hist = histograms[frame]
    
    # Обновляем высоты столбцов
    for i, bar in enumerate(bars):
        bar.set_height(hist[i])
    
    # Обновляем текст
    time_text.set_text(f'Время: {time:.6f} с')
    count_text.set_text(f'Частиц: {len(energies)}')
    
    return bars, time_text, count_text

# Создаем анимацию
ani = FuncAnimation(
    fig, 
    update, 
    frames=len(parsed_data),
    init_func=init,
    blit=False,
    interval=200,  # 200 мс между кадрами
    repeat_delay=2000  # 2 секунды перед повторением
)

plt.tight_layout()
plt.show()

# Для сохранения анимации в файл (раскомментируйте при необходимости)
# ani.save('energy_histogram.gif', writer='pillow', fps=5)