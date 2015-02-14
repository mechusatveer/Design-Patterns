Implementing Read/Write Locks Portably
 
Say for example you have a hash table, that lets you look up strings via a string id:
 
class StringTable
{
   u32         addString( const char* str ) { return mTable.add( str ); }
   const char* getString( u32 id ) const    { return mTable.get( id ); }
 
   HashTable<u32,const char*> mTable;
}
 
Often for classes that are used in multi-threading it would be thread safe to use the 
const functions, but not the non-const ones. And that would be the case here.
 
Now imagine the usage pattern is that 1 in 1000 operations on StringTable are addString(),
and the rest are getString(). All of your threads could be reading from this table 
simultaneously without mutexes, achieving perfect parallelism, except that 1 in 1000 
times the table might be being written to.
 
What if you could lock your mutex for reading and writing separately? Read locks would 
always succeed unless the object is locked for write. Write locks would work like regular 
mutex locks.
 
If all you have are mutexes, you have to lock on both operations, and your shared 
StringTable objects become a lock contention bottleneck.
 
This type if synchronization object is called a read/write lock or a shared exclusive lock.
 
 
 
class KxRWMutex
{
public:
   KxRWMutex( u32 maxReaders = 64 ) :
      mReadsBlocked( false ), mMaxReaders( maxReaders ), mReaders( 0 ) {}
 
   void readLock();
   void readUnlock();
 
   void writeLock();
   void writeUnlock();
 
private:
   void waitReaders();
 
   bool    mReadsBlocked;
   u32     mMaxReaders;
   u32     mReaders;
   KxMutex mReadMutex;
   KxMutex mWriteMutex;
};
 
void KxRWMutex::readLock()
{
   while ( 1 )
   {
      mReadMutex.lock();
      if (( !mReadsBlocked ) && ( mReaders < mMaxReaders ))
      {
         mReaders++;
         mReadMutex.unlock();
         return;
      }
      mReadMutex.unlock();
      kxSleep( 0 );
   }
   assert( 0 );
}
 
void KxRWMutex::readUnlock()
{
   mReadMutex.lock();
   assert( mReaders );
   mReaders--;
   mReadMutex.unlock();
}
 
void KxRWMutex::writeLock()
{
   mWriteMutex.lock();
   waitReaders();
}
 
void KxRWMutex::writeUnlock()
{
   mReadMutex.lock();
   mReadsBlocked = false;
   mReadMutex.unlock();
   mWriteMutex.unlock();
}
 
void KxRWMutex::waitReaders()
{
   // block new readers
   mReadMutex.lock();
   mReadsBlocked = true;
   mReadMutex.unlock();
 
   // wait for current readers to finish
   while ( 1 )
   {
      mReadMutex.lock();
      if ( mReaders == 0 )
      {
         mReadMutex.unlock();
         break;
      }
      mReadMutex.unlock();
      kxSleep( 0 );
   }
   assert( mReaders == 0 );
}
 
class StringTable
{
   u32 addString( const char* str )
   {
      mMutex.writeLock();
      u32 id = mTable.add( str );
      mMutex.writeUnlock();
      return id;
   }
 
   const char* getString( u32 id ) const
   {
      mMutex.readLock();
      const char* str = mTable.get( id );
      mMutex.readUnlock();
      return str;
   }
 
   HashTable<u32,const char*> mTable;
   KxRWMutex                  mMutex;
}
 
