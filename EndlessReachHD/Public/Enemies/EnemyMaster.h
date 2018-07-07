// © 2014 - 2018 Soverance Studios
// http://www.soverance.com

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "GameFramework/Actor.h"
#include "EndlessReachHDPawn.h"
#include "Management/BattleTypes.h"
#include "Management/CommonLibrary.h"
#include "Management/CombatText/CombatTextComponent.h"
#include "EnemyMaster.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FReachedTarget);

UCLASS()
class ENDLESSREACHHD_API AEnemyMaster : public ACharacter
{
	GENERATED_BODY()
		
public:	
	// Sets default values for this actor's properties
	AEnemyMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// What type of battle this enemy initiates (standard, boss, or zhan)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	EBattleTypes BattleType;

	// The name of this enemy as text, for display use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	FText NameText;

	// Enemy Level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	int32 Level;

	// Enemy ATK value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float ATK;

	// Enemy DEF value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float DEF;

	// Enemy Current HP value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float HP_Current;

	// Enemy Max HP value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float HP_Max;

	// The time this enemy will wait between taking turns
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float AttackDelay;

	// The damage output of this enemy. Used to calculate final damage taken by the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
	float DamageOutput;

	/** This emitter is active when the enemy is hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	UParticleSystemComponent* HitFX;
	UParticleSystem* P_HitFX;

	// Hit Audio
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	UAudioComponent* HitAudio;
	USoundCue* S_HitAudio;

	/** This emitter is active when this enemy dies. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects)
	UParticleSystemComponent* DeathFX;
	UParticleSystem* P_DeathFX;

	// Whether or not this character is dead
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls)
	bool bIsDead;

	// Whether or not the character was hit by an attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls)
	bool bIsHit;

	// Whether or not this character is targetable
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Targeting)
	bool bIsTargetable;

	// Do an attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls)
	bool bIsAttacking;

	// Whether or not the enemy is in range of the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls)
	bool bInRange;

	// Whether or not the enemy is currently moving
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls)
	bool bIsMoving;

	// Whether or not the enemy has been aggroed by the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Controls)
	bool bIsAggroed;

	// Enemy's current target. will always be the player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	AEndlessReachHDPawn* Target;

	// The combat text component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Default)
	UCombatTextComponent* CombatTextComponent;

	// Show Combat Damage Text
	UFUNCTION(BlueprintCallable, Category = Combat)
	void ShowCombatDamageText(bool IsCritical, float Damage);

	// Sets the initial stats of the player, based on his level
	UFUNCTION(BlueprintCallable, Category = Stats)
	void SetBaseStats();

	// Sets the initial stats of the player, based on his level
	UFUNCTION(BlueprintCallable, Category = Stats)
	bool ForceHPCaps();

	// Sets the initial stats of the player, based on his level
	UFUNCTION(BlueprintCallable, Category = Combat)
	void EnemyTakeDamage(float DamageTaken);

	// Sets the initial stats of the player, based on his level
	UFUNCTION(BlueprintCallable, Category = Combat)
	float EnemyDealDamage(float BaseATK);

	// Aggro the player. This function is called in Blueprint by the AI OnSeePawn event
	UFUNCTION(BlueprintCallable, Category = Combat)
	void Aggro(APawn* Pawn);

	// Deaggro the player. 
	UFUNCTION(BlueprintCallable, Category = Combat)
	void Deaggro();

	// Ends All Attack Rounds, allowing the enemy A.I. to once again begin searching for the player
	UFUNCTION(BlueprintCallable, Category = Default)
	void EndAttackRound();

	// Event Dispatcher OnDeath
	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FDeath OnDeath;

	// Activate the Death sequence of this enemy
	UFUNCTION(BlueprintCallable, Category = Combat)
	void Death();

	// Activate the Final Death sequence of this enemy, destroying it. Boss enemies will spawn Zhan after this cycle.
	// This function is called at the end of an enemy's Death animation, and must be bound to an AnimNotify in editor
	UFUNCTION(BlueprintCallable, Category = Combat)
	void FinalDeath(bool ShouldDestroy);

	// Activate the Disappear sequence of this enemy
	UFUNCTION(BlueprintCallable, Category = Combat)
	void Disappear();

	// Destroys this enemy
	UFUNCTION(BlueprintCallable, Category = Combat)
	void DestroyEnemy();
};
