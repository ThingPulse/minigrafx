/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 by Daniel Eichhorn
 * Copyright (c) 2019 by Fabrice Weinberg
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "Carousel.h"

Carousel::Carousel(MiniGrafx *miniGrafx, uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
  this->miniGrafx = miniGrafx;
  this->x = x;
  this->y = y;
  this->width = width;
  this->height = height;
}

void Carousel::init() {
  this->miniGrafx->init();
}

void Carousel::setTargetFPS(uint8_t fps){
  float oldInterval = this->updateInterval;
  this->updateInterval = ((float) 1.0 / (float) fps) * 1000;

  // Calculate new ticksPerFrame
  float changeRatio = oldInterval / (float) this->updateInterval;
  this->ticksPerFrame *= changeRatio;
  this->ticksPerTransition *= changeRatio;
}

// -/------ Automatic controll ------\-

void Carousel::enableAutoTransition(){
  this->autoTransition = true;
}
void Carousel::disableAutoTransition(){
  this->autoTransition = false;
}
void Carousel::setAutoTransitionForwards(){
  this->state.frameTransitionDirection = 1;
  this->lastTransitionDirection = 1;
}
void Carousel::setAutoTransitionBackwards(){
  this->state.frameTransitionDirection = -1;
  this->lastTransitionDirection = -1;
}
void Carousel::setTimePerFrame(uint16_t time){
  this->ticksPerFrame = (int) ( (float) time / (float) updateInterval);
}
void Carousel::setTimePerTransition(uint16_t time){
  this->ticksPerTransition = (int) ( (float) time / (float) updateInterval);
}

// -/------ Customize indicator position and style -------\-
void Carousel::enableIndicator(){
  this->state.isIndicatorDrawen = true;
}

void Carousel::disableIndicator(){
  this->state.isIndicatorDrawen = false;
}

void Carousel::enableAllIndicators(){
  this->shouldDrawIndicators = true;
}

void Carousel::disableAllIndicators(){
  this->shouldDrawIndicators = false;
}

void Carousel::setIndicatorPosition(IndicatorPosition pos) {
  this->indicatorPosition = pos;
}
void Carousel::setIndicatorDirection(IndicatorDirection dir) {
  this->indicatorDirection = dir;
}
void Carousel::setActiveSymbol(const char* symbol) {
  this->activeSymbol = symbol;
}
void Carousel::setInactiveSymbol(const char* symbol) {
  this->inactiveSymbol = symbol;
}


// -/----- Frame settings -----\-
void Carousel::setFrameAnimation(AnimationDirection dir) {
  this->frameAnimationDirection = dir;
}
void Carousel::setFrames(FrameCallback* frameFunctions, uint8_t frameCount) {
  this->frameFunctions = frameFunctions;
  this->frameCount     = frameCount;
  this->resetState();
}

// -/----- Overlays ------\-
void Carousel::setOverlays(OverlayCallback* overlayFunctions, uint8_t overlayCount){
  this->overlayFunctions = overlayFunctions;
  this->overlayCount     = overlayCount;
}

// -/----- Loading Process -----\-

void Carousel::setLoadingDrawFunction(LoadingDrawFunction loadingDrawFunction) {
  this->loadingDrawFunction = loadingDrawFunction;
}

void Carousel::runLoadingProcess(LoadingStage* stages, uint8_t stagesCount) {
  uint8_t progress = 0;
  uint8_t increment = 100 / stagesCount;

  for (uint8_t i = 0; i < stagesCount; i++) {
    miniGrafx->clear();
    this->loadingDrawFunction(this->miniGrafx, &stages[i], progress);
    miniGrafx->commit();

    stages[i].callback();

    progress += increment;
    yield();
  }

  miniGrafx->clear();
  this->loadingDrawFunction(this->miniGrafx, &stages[stagesCount-1], progress);
  miniGrafx->commit();

  delay(150);
}

// -/----- Manuel control -----\-
void Carousel::nextFrame() {
  if (this->state.frameState != IN_TRANSITION) {
    this->state.manuelControll = true;
    this->state.frameState = IN_TRANSITION;
    this->state.ticksSinceLastStateSwitch = 0;
    this->lastTransitionDirection = this->state.frameTransitionDirection;
    this->state.frameTransitionDirection = 1;
  }
}
void Carousel::previousFrame() {
  if (this->state.frameState != IN_TRANSITION) {
    this->state.manuelControll = true;
    this->state.frameState = IN_TRANSITION;
    this->state.ticksSinceLastStateSwitch = 0;
    this->lastTransitionDirection = this->state.frameTransitionDirection;
    this->state.frameTransitionDirection = -1;
  }
}

void Carousel::switchToFrame(uint8_t frame) {
  if (frame >= this->frameCount) return;
  this->state.ticksSinceLastStateSwitch = 0;
  if (frame == this->state.currentFrame) return;
  this->state.frameState = FIXED;
  this->state.currentFrame = frame;
  this->state.isIndicatorDrawen = true;
}

void Carousel::transitionToFrame(uint8_t frame) {
  if (frame >= this->frameCount) return;
  this->state.ticksSinceLastStateSwitch = 0;
  if (frame == this->state.currentFrame) return;
  this->nextFrameNumber = frame;
  this->lastTransitionDirection = this->state.frameTransitionDirection;
  this->state.manuelControll = true;
  this->state.frameState = IN_TRANSITION;
  this->state.frameTransitionDirection = frame < this->state.currentFrame ? -1 : 1;
}


// -/----- State information -----\-
CarouselState* Carousel::getUiState(){
  return &this->state;
}


int8_t Carousel::update(){
  long frameStart = millis();
  int8_t timeBudget = this->updateInterval - (frameStart - this->state.lastUpdate);
  if ( timeBudget <= 0) {
    // Implement frame skipping to ensure time budget is keept
    if (this->autoTransition && this->state.lastUpdate != 0) this->state.ticksSinceLastStateSwitch += ceil(-timeBudget / this->updateInterval);

    this->state.lastUpdate = frameStart;
    this->tick();
  } else {
    this->drawFrame();
  }
  return this->updateInterval - (millis() - frameStart);
}


void Carousel::tick() {
  this->state.ticksSinceLastStateSwitch++;

  switch (this->state.frameState) {
    case IN_TRANSITION:
        if (this->state.ticksSinceLastStateSwitch >= this->ticksPerTransition){
          this->state.frameState = FIXED;
          this->state.currentFrame = getNextFrameNumber();
          this->state.ticksSinceLastStateSwitch = 0;
          this->nextFrameNumber = -1;
        }
      break;
    case FIXED:
      // Revert manuelControll
      if (this->state.manuelControll) {
        this->state.frameTransitionDirection = this->lastTransitionDirection;
        this->state.manuelControll = false;
      }
      if (this->state.ticksSinceLastStateSwitch >= this->ticksPerFrame){
          if (this->autoTransition){
            this->state.frameState = IN_TRANSITION;
          }
          this->state.ticksSinceLastStateSwitch = 0;
      }
      break;
  }

  //this->miniGrafx->clear();
  this->drawFrame();
  if (shouldDrawIndicators) {
    this->drawIndicator();
  }
  this->drawOverlays();
  //this->miniGrafx->commit();
}

void Carousel::resetState() {
  this->state.lastUpdate = 0;
  this->state.ticksSinceLastStateSwitch = 0;
  this->state.frameState = FIXED;
  this->state.currentFrame = 0;
  this->state.isIndicatorDrawen = true;
}

void Carousel::drawFrame(){
  switch (this->state.frameState){
     case IN_TRANSITION: {
       float progress = (float) this->state.ticksSinceLastStateSwitch / (float) this->ticksPerTransition;
       int16_t x, y, x1, y1;
       switch(this->frameAnimationDirection){
        case SLIDE_LEFT:
          x = -this->width * progress;
          y = 0;
          x1 = x + this->width;
          y1 = 0;
          break;
        case SLIDE_RIGHT:
          x = this->width * progress;
          y = 0;
          x1 = x - this->width;
          y1 = 0;
          break;
        case SLIDE_UP:
          x = 0;
          y = -this->height * progress;
          x1 = 0;
          y1 = y + this->height;
          break;
        case SLIDE_DOWN:
          x = 0;
          y = this->height * progress;
          x1 = 0;
          y1 = y - this->height;
          break;
         default:
          x = 0;
          y = 0;
          x1 = 0;
          y1 = 0;
       }

       // Invert animation if direction is reversed.
       int8_t dir = this->state.frameTransitionDirection >= 0 ? 1 : -1;
       x *= dir; y *= dir; x1 *= dir; y1 *= dir;

       bool drawenCurrentFrame;


       // Prope each frameFunction for the indicator Drawen state
       this->enableIndicator();
       (this->frameFunctions[this->state.currentFrame])(this->miniGrafx, &this->state, x, y);
       drawenCurrentFrame = this->state.isIndicatorDrawen;

       this->enableIndicator();
       (this->frameFunctions[this->getNextFrameNumber()])(this->miniGrafx, &this->state, x1, y1);

       // Build up the indicatorDrawState
       if (drawenCurrentFrame && !this->state.isIndicatorDrawen) {
         // Drawen now but not next
         this->indicatorDrawState = 2;
       } else if (!drawenCurrentFrame && this->state.isIndicatorDrawen) {
         // Not drawen now but next
         this->indicatorDrawState = 1;
       } else if (!drawenCurrentFrame && !this->state.isIndicatorDrawen) {
         // Not drawen in both frames
         this->indicatorDrawState = 3;
       }

       // If the indicator isn't draw in the current frame
       // reflect it in state.isIndicatorDrawen
       if (!drawenCurrentFrame) this->state.isIndicatorDrawen = false;

       break;
     }
     case FIXED:
      // Always assume that the indicator is drawn!
      // And set indicatorDrawState to "not known yet"
      this->indicatorDrawState = 0;
      this->enableIndicator();
      (this->frameFunctions[this->state.currentFrame])(this->miniGrafx, &this->state, 0, 0);
      break;
  }
}

void Carousel::drawIndicator() {

    // Only draw if the indicator is invisible
    // for both frames or
    // the indiactor is shown and we are IN_TRANSITION
    if (this->indicatorDrawState == 3 || (!this->state.isIndicatorDrawen && this->state.frameState != IN_TRANSITION)) {
      return;
    }

    uint8_t posOfHighlightFrame;
    float indicatorFadeProgress = 0;

    // if the indicator needs to be slided in we want to
    // highlight the next frame in the transition
    uint8_t frameToHighlight = this->indicatorDrawState == 1 ? this->getNextFrameNumber() : this->state.currentFrame;

    // Calculate the frame that needs to be highlighted
    // based on the Direction the indiactor is drawn
    switch (this->indicatorDirection){
      case LEFT_RIGHT:
        posOfHighlightFrame = frameToHighlight;
        break;
      case RIGHT_LEFT:
        posOfHighlightFrame = this->frameCount - frameToHighlight;
        break;
      default:
        posOfHighlightFrame = 0;
    }

    switch (this->indicatorDrawState) {
      case 1: // Indicator was not drawn in this frame but will be in next
        // Slide IN
        indicatorFadeProgress = 1 - ((float) this->state.ticksSinceLastStateSwitch / (float) this->ticksPerTransition);
        break;
      case 2: // Indicator was drawn in this frame but not in next
        // Slide OUT
        indicatorFadeProgress = ((float) this->state.ticksSinceLastStateSwitch / (float) this->ticksPerTransition);
        break;
    }

    uint16_t frameStartPos = (12 * frameCount / 2);
    const char *image;
    uint16_t x,y;
    for (byte i = 0; i < this->frameCount; i++) {

      switch (this->indicatorPosition){
        case TOP:
          y = 0 - (8 * indicatorFadeProgress);
          x = this->width / 2 - frameStartPos + 12 * i;
          break;
        case BOTTOM:
          y = this->height - 10 + (8 * indicatorFadeProgress);
          x = this->width / 2 - frameStartPos + 12 * i;
          break;
        case RIGHT:
          x = this->width -10 + (8 * indicatorFadeProgress);
          y = this->height / 2 - frameStartPos + 2 + 12 * i;
          break;
        case LEFT:
          x = 0 - (8 * indicatorFadeProgress);
          y = this->height / 2 - frameStartPos + 2 + 12 * i;
          break;
        default:
          x = 0;
          y = 0;
      }

      if (posOfHighlightFrame == i) {
         image = this->activeSymbol;
      } else {
         image = this->inactiveSymbol;
      }

      this->miniGrafx->drawXbm(x, y, 8, 8, image);
    }
}

void Carousel::drawOverlays() {
 for (uint8_t i=0;i<this->overlayCount;i++){
    (this->overlayFunctions[i])(this->miniGrafx, &this->state);
 }
}

uint8_t Carousel::getNextFrameNumber(){
  if (this->nextFrameNumber != -1) return this->nextFrameNumber;
  return (this->state.currentFrame + this->frameCount + this->state.frameTransitionDirection) % this->frameCount;
}
