def extract_time_data(filename: str):
    with open(filename, 'r') as f:
        data = f.read().splitlines()
        time_data = []
        iteration_indices = []

        iteration_idx = 1
        for entry in data:
            elems = entry.split()
            if (len(elems) < 2) or (elems[0] != 'Time'):
                continue
            
            time_data.append(elems[2])
            iteration_indices.append(iteration_idx)

            iteration_idx += 1
                    
        return (iteration_indices, time_data)


if __name__ == '__main__':
    (iterations, time_data) = extract_time_data('data/approach1_log.txt')
    (iterations2, time_data2) = extract_time_data('data/approach2_log.txt')    
    (iterations3, time_data3) = extract_time_data('data/approach3_log.txt')    
    (iterations4, time_data4) = extract_time_data('data/approach4_log.txt')    

    for i in range(0, len(iterations)):
        print(time_data[i], end='')
        print(' ', end='')
        print(time_data2[i], end='')
        print(' ', end='')
        print(time_data3[i], end='')
        print(' ', end='')
        print(time_data4[i], end='')
        print()


