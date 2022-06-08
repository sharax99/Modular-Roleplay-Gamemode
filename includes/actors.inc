stock LoadActors()
{
	// Aktorzy

	Cluckin = CreateActor(155, 367.5981, -4.0834, 1001.8516, 180.0);
	SetActorVirtualWorld(Cluckin, 185);

	Lumberjack = CreateActor(161, 815.3389, -612.9754, 16.3359, 23.0452);

	Fisher = CreateActor(184, 2850.8320,-2062.4744,11.5475,89.3808); 

	ArmDealer = CreateActor(6, 0, 0, 0, 353.0);

	Ammunation = CreateActor(179, 1696.6183,-1575.7849,13.8982, 183.6765);
	SetActorVirtualWorld(Ammunation, 748);

	Toolshop = CreateActor(206, 987.0760,-1289.3853,13.5441,188.5148);
	SetActorVirtualWorld(Toolshop, 739);

	BusDriver = CreateActor(253, 2021.1869, 2237.8264, 2104.0459, 3.8541);
	ApplyActorAnimation(BusDriver, "ped", "SEAT_idle", 5, 1, 0, 0, 1, 1);
}