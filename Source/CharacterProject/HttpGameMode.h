// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Http.h"
#include "Json.h"
#include "Dom/JsonObject.h"
#include "HttpGameMode.generated.h"

/**
 * 
 */
UCLASS()
class CHARACTERPROJECT_API AHttpGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHttpGameMode();

	virtual void StartPlay() override;

	//Saving response of the BardAPI with APIResponse
	UPROPERTY(Category = UserInterface, BlueprintReadOnly)
	FString APIResponse;

	//boolean values for each emotions
	UPROPERTY(Category = Emotion, BlueprintReadOnly)
	bool IsHappy;

	UPROPERTY(Category = Emotion, BlueprintReadOnly)
	bool IsDancing;

	UPROPERTY(Category = Emotion, BlueprintReadOnly)
	bool IsSad;

	UPROPERTY(Category = Emotion, BlueprintReadOnly)
	bool IsAngry;

	UPROPERTY(Category = Emotion, BlueprintReadOnly)
	bool IsThinking;

	UPROPERTY(Category = Emotion, BlueprintReadOnly)
	bool IsGreeting;

	UPROPERTY(Category = UserInterface, EditAnywhere, BlueprintReadWrite)
	FString questionToAsk;

	//Getter and Setter for questionToAsk variable
	UFUNCTION(Category = UserInterface, BlueprintCallable)
	FString GetQuestionToAsk();

	UFUNCTION(Category = UserInterface, BlueprintCallable)
	void SetQuestionToAsk(FString NewQuestion);

	//Getter and Setter for APIResponse
	UFUNCTION(Category = UserInterface, BlueprintCallable)
	FString GetAPIResponse();

	UFUNCTION(Category = UserInterface, BlueprintCallable)
	void SetAPIResponse(FString NewAnswer);

	//For Happy Animation
	UFUNCTION(Category = Emotion, BlueprintCallable)
	void SetIsHappy(bool situation);

	//For Dancing Animation
	UFUNCTION(Category = Emotion, BlueprintCallable)
	void SetIsDancing(bool situation);

	//For Greeting Animation
	UFUNCTION(Category = Emotion, BlueprintCallable)
	void SetIsGreeting(bool situation);

	//For Sad Animation
	UFUNCTION(Category = Emotion, BlueprintCallable)
	void SetIsSad(bool situation);

	//For Angry Animation
	UFUNCTION(Category = Emotion, BlueprintCallable)
	void SetIsAngry(bool situation);

	//For Think Animation
	UFUNCTION(Category = Emotion, BlueprintCallable)
	void SetIsThinking(bool situation);

	//Making Bard API Request
	UFUNCTION(Category = UserInterface, BlueprintCallable)
	void MakeHTTPRequest(FString NewQuestion);

	//Enable the animation with the content of the response that Bard gives
	void ProcessResponseText(FString ResponseText);

	//Checking words from the related set and return as true if there is 
	bool CheckWords(FString& ResponseText, TSet<FString>& KeywordSet);

	//Using API from the ElevenLabs for TTS (Text to Speech)
	//UFUNCTION(category = "VoiceCommand", BlueprintCallable)
	//void VoiceAPIRequest();

	//Taking the data of the audio inside the response of ElevenLabs API
	//UPROPERTY(category = "VoiceCommand", BlueprintReadOnly)
	//TArray<uint8> ResponseContent;

	

private:
	//Response of the Bard API
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfull);

	//Response of ElevenLabs API
	//void OnVoiceReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool isSuccessfull);

	//Giving character to the model what they should act like
	FString context = "Express yourself and give answers no more than 100 characters. Also express yourself as one of these feelings: {HAPPY, SAD, ANGRY, THINK}.";

	//Request Url of Bard API
	FString urlRequest = "https://generativelanguage.googleapis.com/v1beta3/models/chat-bison-001:generateMessage?key=AIzaSyBrVFV7gzTRbeSenM12nK-QomVcOAJDB5g";

	//Keyword Sets for each emotion
	TSet<FString> HappyKeywords;
	TSet<FString> SadKeywords;
	TSet<FString> AngryKeywords;
	TSet<FString> ThinkKeywords;
	TSet<FString> DanceKeywords;
	TSet<FString> GreetKeywords;

};
