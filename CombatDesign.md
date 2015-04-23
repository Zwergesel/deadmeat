stats are used for the training rate of a skill
higher Vit makes the health skill easier/faster to train etc.

**max\_health** = health skill

**attack** = (weapon to hit + weapon enchantment) + ((fighting skill + weapon skill)/2)

**defense** = armor + (fighting skill + armor skill)/2 + tile defender is on

**damage** = (weapon damage + weapon enchantment)

**speed** = base attack time - weapon speed + armor hindrance

**attack value** = attack - defense

x = gauss on range (-300, 300) with attack value as mean

x < -70 = miss, -70 < x < 0 = half damage, 0 < x < 175 = full damage, 175 < x = critical hit

mean 0 => miss 25%, half 25%, hit 46%, crit 4%