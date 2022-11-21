#include "ipc.h"

int main() {
    JobRequestBuffer_t* job_buffer = ipc_get_job_buffer();
    if (!job_buffer) {
        return -1;
    }

    job_buffer->cmd = CMD_KILL_SERVER;
    submit_job_request(job_buffer);
    wait_for_job_completion(job_buffer);

    return 0;
}
