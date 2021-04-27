for(int i = 0;i < 3;i++){
		tRect.x = i * gBush.getWidth()/3;
		tRect.y = 0;
		tRect.w = gBush.getWidth()/3,tRect.h = gBush.getHeight()/3;
		gBushSprite.push_back(tRect);
	}