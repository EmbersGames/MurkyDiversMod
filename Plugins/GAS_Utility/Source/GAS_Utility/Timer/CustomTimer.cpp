

#include "CustomTimer.h"

int UCustomTimer::_TIMERID = 0;
UWorld* UCustomTimer::_world = nullptr;
UGameInstance* UCustomTimer::_gameInstance = nullptr;
bool UCustomTimer::_isInSeemLessTravel = false;

TMap<int, UCustomTimer*> timersMap = TMap<int, UCustomTimer*>();

UCustomTimer::UCustomTimer() : Super()
{
	timerID = _TIMERID++;
	
	startTime = -1.0f;
	isWorking = false;
	lastResumeTime = -1.0f;
	elapsedTime = 0.0f;
	lastElapsedTime = 0.0f;
	Paused = false;
}

UCustomTimer::~UCustomTimer()
{
	timersMap.Remove(timerID);
}

UCustomTimer* UCustomTimer::createTimer(UObject* WorldContextObject)
{
	UCustomTimer* timer = NewObject<UCustomTimer>();
	if (timer == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Warning : CustomTimer creation failled in static method createTimer !"));
		return nullptr;
	}

	if (_world == nullptr)
		setWorldReferenceForAllTimers(WorldContextObject);
	
	timersMap.Add(timer->getID(), timer);

	timer->Start(WorldContextObject);
	return timer; 
}

void UCustomTimer::createTimerIfNull(UObject* WorldContextObject, UPARAM(Ref) UCustomTimer*& timer)
{
	if (!IsValid(timer))
	{
		timer = createTimer(WorldContextObject);
	}
}

void UCustomTimer::createOrRestartTimer(UObject* WorldContextObject, UPARAM(Ref) UCustomTimer*& timer, float timeOffset)
{
	if (!IsValid(timer))
	{
		timer = createTimer(WorldContextObject);
	}
	if(IsValid(timer))
	{
		timer->restart(WorldContextObject);
		timer->setTimeOffset(timeOffset);
	}
}

void UCustomTimer::setWorldReferenceForAllTimers(UObject* WorldContextObject)
{
	if(WorldContextObject){
		UWorld* newWorldRef = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull);
		if (_world != newWorldRef && IsValid(newWorldRef))
			_world = newWorldRef;
	}
	else {
		if (GEngine->GetWorld() != nullptr)
		{
			UWorld* newWorldRef = GEngine->GetWorld();
			if (_world != newWorldRef && IsValid(newWorldRef))
				_world = newWorldRef;
		}
		else {
			if (_gameInstance != nullptr && IsValid(_gameInstance)) {
				UWorld* newWorldRef = _gameInstance->GetWorld();
				if (_world != newWorldRef && IsValid(newWorldRef))
					_world = newWorldRef;
			}

			
		}

	}
}

void UCustomTimer::setNullWorldReferenceForAllTimers()
{
	_world = nullptr;
}

UWorld* UCustomTimer::getWorldReferenceOfAllTimers()
{
	return _world;
}

void UCustomTimer::enterSeamlessTravelState()
{
	_world = nullptr;
	_isInSeemLessTravel = true;
}

void UCustomTimer::exitSeamlessTravelState(UObject* WorldContextObject)
{
	_world = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	_isInSeemLessTravel = false;
}

bool UCustomTimer::isInSeemLessTravelState()
{
	return _isInSeemLessTravel;
}


void UCustomTimer::Start(UObject* WorldContextObject)
{

	if (!isWorking)
	{
		setWorldReferenceForAllTimers(WorldContextObject);
		if (!IsValid(_world))
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning : world reference is not valid in Start !"));
			return;
		}
		else if (!isWorking)
		{

			if (startTime == -1)
			{
				startTime = _world->GetTimeSeconds();
			}
			else
			{
				lastResumeTime = _world->GetTimeSeconds();
			}

			isWorking = true;
		}
	}
	Paused = false;
}

void UCustomTimer::Pause()
{
	if (isWorking)
	{
		if (!IsValid(_world))
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning : world reference is not valid in Pause !"));
			return;
		}

		isWorking = false;
		Paused = true;
		lastElapsedTime = elapsedTime;
	}
}

float UCustomTimer::Stop(UObject* WorldContextObject)
{
	float tempElapsedTime = 0.0f;
	if (isWorking)
	{
		setWorldReferenceForAllTimers(WorldContextObject);
		if (!IsValid(_world))
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning : world reference is not valid in Stop !"));
			return -1.0f;
		}

		if (lastResumeTime == -1)
		{
			tempElapsedTime = _world->GetTimeSeconds() - startTime;
		}
		else
		{
			tempElapsedTime = lastElapsedTime + (_world->GetTimeSeconds() - lastResumeTime);
		}

		startTime = -1.0f;
		isWorking = false;
		lastResumeTime = -1.0f;
		elapsedTime = 0.0f;
		Offset = 0.0f;
	}
	else 
	{
		setWorldReferenceForAllTimers(WorldContextObject);
		if (!IsValid(_world))
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning : world reference is not valid in Stop !"));
			return -1.0f;
		}

		if (lastResumeTime != -1)
		{
			tempElapsedTime = lastElapsedTime + (_world->GetTimeSeconds() - lastResumeTime);
		}
		
		startTime = -1.0f;
		isWorking = false;
		lastResumeTime = -1.0f;
		elapsedTime = 0.0f;
		Offset = 0.0f;
	}
	
	Paused = false;

	return tempElapsedTime;
}


void UCustomTimer::restart(UObject* WorldContextObject)
{
	setWorldReferenceForAllTimers(WorldContextObject);
	if (!IsValid(_world))
	{
		UE_LOG(LogTemp, Warning, TEXT("Warning : world reference is not valid in restart !"));
		return;
	}

	startTime = _world->GetTimeSeconds();
	isWorking = true;
	lastResumeTime = -1.0f;
	elapsedTime = 0.0f;
	lastElapsedTime = 0.0f;
	Offset = 0.0f;
	Paused = false;
}


int UCustomTimer::getID()
{
	return timerID;
}

float UCustomTimer::getElapsedTime(UObject* WorldContextObject)
{
	if (_isInSeemLessTravel) {

		UE_LOG(LogTemp, Warning, TEXT("Warning : getElapsedTime in seamless travel !"));
		if (lastResumeTime == -1) // Timer was never paused
		{
			return 0;
		}
		return lastElapsedTime + Offset;
	}
	setWorldReferenceForAllTimers(WorldContextObject);
	if (!IsValid(_world))
	{
		UE_LOG(LogTemp, Warning, TEXT("Warning : world reference is not valid in getElapsedTime !"));
		return -1.0f;
	}
	if (isWorking)
	{
		if (lastResumeTime == -1) // Timer was never paused
		{
			elapsedTime = _world->GetTimeSeconds() - startTime;
		}
		else // Timer was paused at least once
		{
			elapsedTime = lastElapsedTime + (_world->GetTimeSeconds() - lastResumeTime);
		}
	}
	else
	{
		if (startTime == -1)
		{
			return -1;
		}
	}

	return elapsedTime + Offset;
}

bool UCustomTimer::isRunning()
{
	return isWorking;
}

bool UCustomTimer::IsPaused()
{
	return Paused;
}


void UCustomTimer::setTimeOffset(float inOffset)
{
	Offset = inOffset;
}

float UCustomTimer::getTimeOffset()
{
	return Offset;
}
