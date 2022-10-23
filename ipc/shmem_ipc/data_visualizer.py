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
    data_dir = input('enter the path to the data directory: ')
    (iterations, time_data) = extract_time_data('{}/approach1_log.txt'.format(data_dir))
    (iterations2, time_data2) = extract_time_data('{}/approach2_log.txt'.format(data_dir))
    (iterations3, time_data3) = extract_time_data('{}/approach3_log.txt'.format(data_dir))
    (iterations4, time_data4) = extract_time_data('{}/approach4_log.txt'.format(data_dir))

    with open('{}/results.csv'.format(data_dir), 'w') as f:
        f.write('Iteration,Independent Client (normal),Independent Client (elevated),Client + Server (normal),Client + Server (elevated)\n')
        for i in range(0, len(iterations)):
            f.write(str(i + 1))
            f.write(',')
            f.write(str(time_data[i]))
            f.write(',')
            f.write(str(time_data2[i]))
            f.write(',')
            f.write(str(time_data3[i]))
            f.write(',')
            f.write(str(time_data4[i]))
            f.write('\n')

