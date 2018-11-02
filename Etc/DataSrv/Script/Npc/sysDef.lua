target = User.get(userID)
inventory = target->inventory()

--Npc.get() will only return a unique pointer in a session.
self = Npc.get()