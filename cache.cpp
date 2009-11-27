#ifdef MULTITHREADING
MUTEX cacheMutex;
#endif

#ifdef SPLAY
#include "cache_splay.cpp"
#else
#include "cache_hash.cpp"
#endif

#ifndef MULTITHREADING
#undef THREADS
#define THREADS 1
#endif
#define CACHE_TRIM_THRESHOLD (CACHE_SIZE-(X*Y*2*THREADS))

#ifdef MULTITHREADING
int threadsRunning = 0, threadsReady = 0;
CONDITION trimReady, trimDone;
#endif

void postNode()
{
	if (cacheSize >= CACHE_TRIM_THRESHOLD)
	{
#ifdef MULTITHREADING
		// (re)create barrier, set trimPending
		SCOPED_LOCK lock(cacheMutex);

		if (cacheSize >= CACHE_TRIM_THRESHOLD) // synchronized check
		{
			if (threadsReady==0)
			{
				threadsReady = 1;
				while (threadsReady < threadsRunning)
					trimReady.wait(lock);
				cacheTrim();
				assert(cacheSize < CACHE_TRIM_THRESHOLD, "Trim failed");
				threadsReady = 0;
				trimDone.notify_all();
			}
			else
			{
				threadsReady++;
				trimReady.notify_all();
				trimDone.wait(lock);
				assert(cacheSize < CACHE_TRIM_THRESHOLD, "Trim failed");
			}

		}
#else
		cacheTrim();
		assert(cacheSize < CACHE_TRIM_THRESHOLD, "Trim failed");
#endif
	}
}

void onThreadExit()
{
#ifdef MULTITHREADING
	/* LOCK */
	{
		SCOPED_LOCK lock(cacheMutex);
		threadsRunning--;
		trimReady.notify_all();
	}
#endif
}

INLINE void markDirty(Node* np)
{
	((CacheNode*)np)->dirty = true;
}

#ifndef HAVE_CACHE_PEEK

#define PEEK_BUF_SIZE 102400
Node peekBuf[PEEK_BUF_SIZE];
NODEI peekPos = 0;
#ifdef MULTITHREADING
MUTEX peekMutex;
#endif

INLINE const Node* cachePeek(NODEI index)
{
	NODEI p;
	/* LOCK */
	{
#ifdef MULTITHREADING
		SCOPED_LOCK lock(peekMutex);
#endif
		p = peekPos++;
	}
	cacheUnarchive(index, &peekBuf[p]);
	return &peekBuf[p];
}

#endif // HAVE_CACHE_PEEK