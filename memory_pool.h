#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__
#include <list>
#include <malloc.h>
template<class T>
class MemoryPool
{
	typedef enum{USED, IDLE}STATE_TYPE;
	typedef struct _ST_MEMORY{
		STATE_TYPE state;
		T* buffer;
		size_t length;
		_ST_MEMORY():state(IDLE),buffer(NULL),length(0){}
	}ST_MEMORY;
public:
	MemoryPool(size_t totalsize = 20*1024*1024):m_totalsize(totalsize)
	{
		m_address = (T*)malloc(sizeof(T)*totalsize);
		memset(m_address, 0, sizeof(T)*totalsize);
		if (m_address){
			ST_MEMORY memory;
			memory.buffer = m_address;
			memory.length = m_totalsize;
			memory.state = IDLE;
			m_memorylist.push_back(memory);
		}
		else{
			printf("the memory pool malloc failed!!!!!!!!\n");
		}
	}
	~MemoryPool()
	{
		if (m_address){
			free(m_address);
			m_address = NULL;
		}
		m_memorylist.clear();
	}
	inline T* allocate(size_t blocksize)
	{
		T* buffer = NULL;
		if (blocksize == 0 || blocksize > m_totalsize || m_memorylist.empty())
			return buffer;
		for (std::list<ST_MEMORY>::iterator ite = m_memorylist.begin(); ite != m_memorylist.end(); ite++)
		{
			if (ite->state == IDLE && ite->length > blocksize){
				ST_MEMORY memory;
				buffer = ite->buffer;
				memory.buffer = ite->buffer;
				memory.state = USED;
				memory.length = blocksize;
				m_memorylist.push_back(memory);

				ite->buffer = ite->buffer+blocksize;
				ite->length -= blocksize;
				break;
			}
			else if (ite->state == IDLE && ite->length == blocksize){
				buffer = ite->buffer;
				ite->state = USED;
				break;
			}
		}
		memset(buffer, 0, sizeof(T)*blocksize);
		return buffer;
	}
	inline void deallocate(T* buffer)
	{
		if (!buffer && m_memorylist.empty())
			return;

		ST_MEMORY current;
		for (std::list<ST_MEMORY>::iterator ite = m_memorylist.begin(); ite != m_memorylist.end(); ite++)
		{
			if (ite->buffer == buffer){
				current.buffer = ite->buffer;
				current.length = ite->length;
				current.state = IDLE;
				m_memorylist.erase(ite);
				break;
			}
		}

		if (current.buffer == NULL && current.length == 0)
			return;

		std::list<ST_MEMORY>::iterator ite = m_memorylist.begin();
		while (ite != m_memorylist.end())
		{
			if (ite->state == IDLE && ite->buffer+ite->length == buffer){
				current.length += ite->length;
				current.buffer = ite->buffer;
				ite = m_memorylist.erase(ite);
				continue;
			}
			if (ite->state == IDLE && ite->buffer == current.buffer+current.length){
				current.length+=ite->length;
				ite = m_memorylist.erase(ite);
				continue;
			}
			ite++;
		}
		m_memorylist.push_back(current);
	}
private:
	size_t m_totalsize;
	std::list<ST_MEMORY> m_memorylist;
	T* m_address;
};
#endif /*__MEMORY_POOL_H__*/