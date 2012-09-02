/**
 * provide a simple stopwatch implementation, based on clock_gettime()
 */
/* compile with gcc -Wall -Werror -pedantic -std=gnu99 -lrt 
 * additionally use -DTEST_STOPWATCH to compile the main() */

#include <stdio.h>
#include <string.h>

#include "stopwatch.h"
#include "utils.h"

/*
 * Utility function to manipulate timespec
 */

/**
 * compare two timespecs
 * @returns: an integer greater than, equal to, or less
 *      than 0 if the timespec structure pointed by a is greater than, equal to,
 *      or less than the timespec structure pointed by b, respectively.
 *      if one pointer is NULL, it is interpreted as negative infinity in time
 */
long timespeccmp(const struct timespec* a, const struct timespec* b){
    long a_tvsec, a_tvnsec, b_tvsec, b_tvnsec;
    if (a == NULL && b == NULL) { return 0;  }
    if (a == NULL && b != NULL) { return -1; }
    if (a != NULL && b == NULL) { return 1;  }

    /* get the nanoseconds in [0, 1000000000[ to have lexical ordering */
    a_tvsec = a->tv_sec + a->tv_nsec / 1000000000L + (a->tv_nsec < 0);
        /* add one second if the nanoseconds are negative, because modulus will
         * still be negative */
    a_tvnsec = a->tv_nsec >= 0L ?    a->tv_nsec % 1000000000L : 
                                    1000000000L - a->tv_nsec % 1000000000L;
                                    
    b_tvsec = b->tv_sec + b->tv_nsec / 1000000000L + (b->tv_nsec < 0);
    b_tvnsec = b->tv_nsec >= 0L ?    b->tv_nsec % 1000000000L : 
                                    1000000000L - b->tv_nsec % 1000000000L;

    if ( a_tvsec != b_tvsec ){  return a_tvsec - b_tvsec;
    } else {                    return a_tvnsec - b_tvnsec; }
}

/**
 * compares the timespec against zero
 * semantically equivalent to timespeccmp(a, TIMESPEC_ZERO)
 */
long timespeccmp0(const struct timespec* a){
    const struct timespec b = { .tv_sec = 0, .tv_nsec = 0L};
    return timespeccmp(a, &b);
}

/**
 * returns the difference between end and start, end being after start
 * @pre: both start and end are valid pointers
 *       end is after start
 * @post: a newly malloc'ed struct containing the difference is returned,
 *       or NULL in case of an error (errno is set to ENOMEM)
 */
struct timespec* diff_time(struct timespec* start, struct timespec* end) {
    struct timespec* temp;
    if ((temp = (struct timespec*) malloc(sizeof(struct timespec))) == NULL){
        fprintf(stderr, 
            "Malloc error on allocation memory for the time difference\n");
        return NULL;
    }
    if ((end->tv_nsec - start->tv_nsec) < 0) {
        temp->tv_sec = end->tv_sec - start->tv_sec - 1;
        temp->tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
    } else {
        temp->tv_sec = end->tv_sec-start->tv_sec;
        temp->tv_nsec = end->tv_nsec-start->tv_nsec;
    }
    return temp;
}

/**
 * Adds B to A
 */
void time_addtoA(struct timespec* A, struct timespec* B){
    if ((A->tv_nsec + B->tv_nsec) >= 1000000000) {
        A->tv_sec = A->tv_sec + B->tv_sec + 1;
        A->tv_nsec = A->tv_nsec + B->tv_nsec - 1000000000;
    } else {
        A->tv_sec = A->tv_sec + B->tv_sec;
        A->tv_nsec = A->tv_nsec + B->tv_nsec;
    }
}

/**
 * initializes a stopwatch with clockid_t CLOCK_REALTIME
 * @return: NULL on failure
 */
stopwatch* sw_init(){
    return sw_init_clocktype(CLOCK_REALTIME);
}

/**
 * initializes a stopwatch with specified clockid.
 * @pre: the clockid_t must be a valid one.
 *      Interesting values include:
 *          - CLOCK_REALTIME : use real time
 *          - CLOCK_PROCESS_CPUTIME_ID : the time effectively used on the
 *              processor by this process
 *      For more values, see man 3posix clock_gettime
 *      enough memory should be available.
 * @post: a new stopwatch is allocated and initialized correctly.
 * @return: the pointer to that instance. On error, NULL is returned, and errno
 *      is set appropriately.
 */
stopwatch* sw_init_clocktype(clockid_t id){
    stopwatch* ret;
    struct timespec clockid_test;
    if (clock_gettime(id, &clockid_test) != 0){
        DEBUGPRINT2("Invalid clock id");
        return NULL;
    }
    CHECKNULL((ret = (stopwatch*)malloc(sizeof(stopwatch))), NULL);
    memset(ret, 0, sizeof(stopwatch));
    ret->clock_type = id;
    return ret;
}

/**
 * Updates the fields total_elapsed time, min_startstop and max_startstop
 * against stoptime.
 * @pre: the stopwatch must be started, that is laststart shouldn't be 0
 *       no NULL argument
 * @post: the fields are updated
 * @return: 0 on success, -1 on failure
 */
static int update_time_stats(stopwatch* sw, struct timespec* stoptime){
    struct timespec* delta;
    CHECKNULL(sw, -1);
    CHECKNULL(stoptime, -1);
    if (timespeccmp0(&(sw->laststart)) == 0){
        DEBUGPRINT2("sw.laststart is 0: the stopwatch was not started");
        return -1;
    }
    if ((delta = diff_time(&(sw->laststart), stoptime)) == NULL){
        /* already printed debug mesg and errno is set to ENOMEM */
        return -1;
    }
    time_addtoA(&(sw->total_elapsed), delta);
    if (timespeccmp(&(sw->min_startstop), delta) > 0 
            || timespeccmp0(&(sw->min_startstop)) == 0){
        memcpy(&(sw->min_startstop), delta, sizeof(struct timespec));
    }
    if (timespeccmp(&(sw->max_startstop), delta) < 0){
        memcpy(&(sw->max_startstop), delta, sizeof(struct timespec));
    }
    free(delta);
    return 0;
}

/**
 * (re-)start stopwatch
 * @pre: sw is a valid non-NULL reference to a stopwatch
 *      sw might be either stopped or started.
 * @post: the stopwatch is started.
 *      in case of an already started stopwatch, the different statistics are
 *      updated, and the last start (the time reference) is set to the current
 *      time value.
 * @return: 0 on success, -1 on error
 */
int sw_start(stopwatch* sw){
    struct timespec starttime;
    CHECKNULL(sw, -1);
    if (clock_gettime(sw->clock_type, &starttime) != 0){
        DEBUGPRINT2("Error on clock_gettime(): probably invalid clock_id\n");
        return -1;
    }
    /* first start or start after stop */
    if (timespeccmp0((&sw->laststart)) == 0 ){
        memcpy(&(sw->laststart), &starttime, sizeof(struct timespec));
        sw->nstarts += 1;
        return 0;
    }
    if (update_time_stats(sw, &starttime) != 0){
        return -1;
    }
    /* reset starting time */
    memcpy(&(sw->laststart), &starttime, sizeof(struct timespec));
    sw->nstarts += 1;
    return 0;
}

/**
 * stops stopwatch
 * @pre: sw is a valid non-NULL reference to a stopwatch
 *      sw might be either stopped or started.
 * @post: the stopwatch is stopped. the different statistics are updated
 * @return: 0 on success, -1 on error
 */
int sw_stop(stopwatch* sw){
    struct timespec stoptime;
    CHECKNULL(sw, -1);
    if (clock_gettime(sw->clock_type, &stoptime) != 0){
        DEBUGPRINT2("Error on clock_gettime(): probably invalid clock_id\n");
        return -1;
    }
    if (timespeccmp0(&(sw->laststart)) == 0){
        /* already stopped */
        return 0;
    }
    if (update_time_stats(sw, &stoptime) != 0){
        return -1;
    }
    /* reset the starting time */
    memset(&(sw->laststart), 0, sizeof(struct timespec));
    return 0;
}

/**
 * prints statistics about stopwatch
 * @pre: sw is not NULL
 * @post: some statistics (total measured time, minimal interval recorded,
 *      maximal interval, total number of starts and average duration of recording
 *      intervals) are printed on stdout
 * @return: 0 on success, -1 on error
 */
int sw_print(stopwatch* sw){
    CHECKNULL(sw, -1);
    if (printf("total measured time: %lis, %lins\n", sw->total_elapsed.tv_sec,
            sw->total_elapsed.tv_nsec) < 0){
        return -1;
    }
    if (printf("minimum measurement: %lis, %lins\n", sw->min_startstop.tv_sec,
            sw->min_startstop.tv_nsec) < 0){
        return -1;
    }
    if (printf("maximum measurement: %lis, %lins\n", sw->max_startstop.tv_sec,
            sw->max_startstop.tv_nsec) < 0){
        return -1;
    }
    if (printf("number of measurements: %lu\n", sw->nstarts) < 0){
        /* actually number of starts */
        return -1;
    }
    if (sw->nstarts == 0){
        if (printf("average measurement: NaN\n") < 0){
            return -1;
        }
    } else if (timespeccmp0(&(sw->laststart)) == 0){ /* stopped */
        if (printf("average measurement: %lis, %lins\n",
                sw->total_elapsed.tv_sec / sw->nstarts,
                (long)((sw->total_elapsed.tv_sec % sw-> nstarts) / (double)
                sw->nstarts  * 1000000000L )
                + sw->total_elapsed.tv_nsec / sw->nstarts ) < 0){
            return -1;
        }
    } else if (sw->nstarts == 1){ /* stopwatch not yet stopped/restarted */
        if (printf("average measurement: NaN\n") < 0){
            return -1;
        }
    } else { /* stopwatch running: mean on sw->nstarts - 1 */
        if (printf("average measurement: %lis, %lins\n",
                sw->total_elapsed.tv_sec / (sw->nstarts - 1),
                (long)((sw->total_elapsed.tv_sec % (sw-> nstarts - 1)) / (double)
                (sw->nstarts - 1) * 1000000000L )
                + sw->total_elapsed.tv_nsec / (sw->nstarts - 1)) < 0){
            return -1;
        }
    }
    return 0;
}

/**
 * free ressources used by stopwatch
 * @pre: sw is either a valid stopwatch reference, or NULL
 * @post: if not NULL, the ressources used by sw are free()'ed.
 *      if NULL, nothing is done
 */
void sw_free(stopwatch* sw){
    CHECKNULL(sw, );
    free(sw);
}

#ifdef TEST_STOPWATCH
int main(){
    stopwatch* sw;
    sw = sw_init();
    
    for (int n = 0; n < 11; n++){
        sw_start(sw);
        long temp=1;
        for (int i = 0; i< 242000000; i++)
            temp+=temp;
    }
    sw_print(sw);
    sw_stop(sw);
    sw_free(sw);
    return 0;
}
#endif /* TEST_STOPWATCH */
