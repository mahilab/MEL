from Mutex import Mutex

b = Mutex("my_mutex")

b.try_lock()
print "B can continue now"
b.release()
