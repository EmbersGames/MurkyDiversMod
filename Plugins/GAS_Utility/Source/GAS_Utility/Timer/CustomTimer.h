

#pragma once

#include "CoreMinimal.h"
#include <iostream>
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Engine.h"
#include "Engine/GameInstance.h"
#include "CustomTimer.generated.h"


/**
 * \Class UCustomTimer
 * \brief Timer that can be paused and resumed at will
 **/

UCLASS(BlueprintType)
class  UCustomTimer : public UObject
{
	GENERATED_BODY()
		
		// Attributes
private:
	UPROPERTY()
	int timerID;
	UPROPERTY()
	float startTime;
	UPROPERTY()
	float lastResumeTime;
	UPROPERTY()
	float elapsedTime;
	UPROPERTY()
	float lastElapsedTime;
	UPROPERTY()
	bool isWorking;
	UPROPERTY()
	bool Paused;
	UPROPERTY()
	float Offset;

	static int _TIMERID;
	static UWorld* _world;
	static bool _isInSeemLessTravel;
	

		// Constructors, destructors
public:
	static UGameInstance* _gameInstance;
	/**
	* Default constructor of the class
	**/
	UCustomTimer();


	/**
	* Default destructor of the class
	**/
	~UCustomTimer();


// Static management of timers
public:


	/**
	* Timer creator for blueprint usage, create a timer and start it
	* /!\ Do NOT use it in constructor /!\
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static UCustomTimer* createTimer(UObject* WorldContextObject);

	/**
	* Create and set reference to the given reference if timer is null
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void createTimerIfNull(UObject* WorldContextObject, UPARAM(Ref) UCustomTimer*& timer);

	/**
	* Create timer if the reference is null or reset it if it is valid
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void createOrRestartTimer(UObject* WorldContextObject, UPARAM(Ref) UCustomTimer*& timer, float TimeOffset = 0.f);


	/**
	* Set the newWorldRef as the world reference for all custom timers
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void setWorldReferenceForAllTimers(UObject* WorldContextObject);


	/**
	* Invalidate the world reference for all custom timers by setting it to null reference
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void setNullWorldReferenceForAllTimers();


	/**
	* Return the current world reference of all custom timers
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static")
	static UWorld* getWorldReferenceOfAllTimers();


	/**
	* Change the state of all custom timers to inSeemLessTravel
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static")
	static void enterSeamlessTravelState();


	/**
	* Change the state of all custom timers to inNormalGame
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	static void exitSeamlessTravelState(UObject* WorldContextObject);


	/**
	* Change the state of all custom timers to inNormalGame
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Static")
	static bool isInSeemLessTravelState();


	// Methods
public:
	/**
	* Start the timer after it was pasued or after it was created
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Usage", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	void Start(UObject* WorldContextObject = nullptr);


	/**
	* Stop the timer immediatly and save current time elapsed
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Usage")
	void Pause();


	/**
	* Stop the timer immediatly and reset time elapsed and begin time
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Usage", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	float Stop(UObject* WorldContextObject = nullptr);


	/**
	* Stop the timer immediatly, reset values and start timer again
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Usage", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	void restart(UObject* WorldContextObject = nullptr);


	/**
	* \brief Return the timer id
	**/
	UFUNCTION(BlueprintPure, Category = "CustomTimer|Usage")
	int getID();


	/**
	* \brief return elapsed time since last activation
	**/
	UFUNCTION(BlueprintPure, Category = "CustomTimer|Usage", meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"))
	float getElapsedTime(UObject* WorldContextObject = nullptr);


	/**
	* \brief Return true if timer is active
	**/
	UFUNCTION(BlueprintPure, Category = "CustomTimer|Usage")
	bool isRunning();
	
	/**
	* \brief Return true if timer is paused
	**/
	UFUNCTION(BlueprintPure, Category = "CustomTimer|Usage")
	bool IsPaused();


	/**
	* \brief set time offset for timer
	**/
	UFUNCTION(BlueprintCallable, Category = "CustomTimer|Usage")
	void setTimeOffset(float inOffset);


	/**
	* \brief get time offset for timer
	**/
	UFUNCTION(BlueprintPure, Category = "CustomTimer|Usage")
	float getTimeOffset();

};


/**
* global reference to all custom timer, only one can exist at a time
**/
extern TMap<int, UCustomTimer*> timersMap;