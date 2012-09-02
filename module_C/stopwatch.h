/**
 * provide a simple stopwatch implementation, based on clock_gettime()
 */
#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <time.h>
#include <stdlib.h>

typedef struct sw {
    struct timespec laststart;     /* the time of the last start, if the stopwatch hasn't been stopped since */
    struct timespec total_elapsed; /* the total measured time */
    struct timespec min_startstop; /* the minimal interval recorded between a start and a stop or another start */
    struct timespec max_startstop; /* the maximal interval */
    unsigned long nstarts;         /* the total number of times this stopwatch has been started */
    clockid_t clock_type;           /* clock type : see the sw_init methods */
} stopwatch;

/**
 * initializes a stopwatch with clockid_t CLOCK_REALTIME
 * @return: NULL on failure
 */
stopwatch* sw_init();

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
stopwatch* sw_init_clocktype(clockid_t t);

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
int sw_start(stopwatch* sw);

/**
 * stops stopwatch
 * @pre: sw is a valid non-NULL reference to a stopwatch
 *      sw might be either stopped or started.
 * @post: the stopwatch is stopped. the different statistics are updated
 * @return: 0 on success, -1 on error
 */
int sw_stop(stopwatch* sw);

/**
 * prints statistics about stopwatch
 * @pre: sw is not NULL
 * @post: some statistics (total measured time, minimal interval recorded,
 *      maximal interval, total number of starts and average duration of recording
 *      intervals) are printed on stdout
 * @return: 0 on success, -1 on error
 */
int sw_print(stopwatch* sw);

/**
 * free ressources used by stopwatch
 * @pre: sw is either a valid stopwatch reference, or NULL
 * @post: if not NULL, the ressources used by sw are free()'ed.
 *      if NULL, nothing is done
 */
void sw_free(stopwatch* sw);


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
long timespeccmp(const struct timespec* a, const struct timespec* b);

/**
 * compares the timespec against zero
 * semantically equivalent to timespeccmp(a, TIMESPEC_ZERO)
 */
long timespeccmp0(const struct timespec* a);

/**
 * returns the difference between end and start, end being after start
 * @pre: both start and end are valid pointers
 *       end is after start
 * @post: a newly malloc'ed struct containing the difference is returned,
 *       or NULL in case of an error (errno is set to ENOMEM)
 */
struct timespec* diff_time(struct timespec* start, struct timespec* end);

/**
 * Adds B to A
 */
void time_addtoA(struct timespec* A, struct timespec* B);

#endif /* STOPWATCH_H */
