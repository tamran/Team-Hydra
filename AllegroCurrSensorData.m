clear; format short
axesSize = 18;
tickSize = 12;
titleSize = 18;
markerSize = 8;
legSize = 16;
lw = 1;

%% data
curr_pos = [0 1 2 3 4 5 5.9]; % in A
signal_pos = [1.649 1.655 1.662 1.669 1.675 1.683 1.688]; % in V
curr_neg = [-1 -2 -3 -4 -5]; % in A
signal_neg = [1.629 1.626 1.618 1.610 1.604]; % in V
curr = [curr_pos curr_neg]; % in A
signal = [signal_pos signal_neg]; % in V

clf; figure(1)
plot(curr,signal,'bo','MarkerFaceColor','blue')
set(gca,'FontSize',tickSize)
grid on
xlabel('Current (A)','FontSize',axesSize)
ylabel('Signal (V)','FontSize',axesSize)
title('Signal vs. Current','FontSize',titleSize)
hold on

% linear fit
P = polyfit(curr,signal,1);
modelCurr = -10:.5:10;
modelSignal = polyval(P,modelCurr);

% slope is the EFFECTIVE resistance of the current sensor
R = P(1)  
% the actual resistor should be 100 ?m

plot(modelCurr,modelSignal,'k-');

% linear fit for postive and negative current separately
P_pos = polyfit(curr_pos, signal_pos,1);
R_pos = P_pos(1)
modelCurr_pos = 0:.5:10;
modelSignal_pos = polyval(P_pos,modelCurr_pos);

P_neg = polyfit(curr_neg, signal_neg, 1);
R_neg = P_neg(1)
modelCurr_neg = -10:.5:0;
modelSignal_neg = polyval(P_neg,modelCurr_neg);

plot(modelCurr_pos, modelSignal_pos, 'r-')
plot(modelCurr_neg, modelSignal_neg, 'b-')

totalFitLeg = sprintf('total fit, R_{eff} = %.4g',R);
posFitLeg = sprintf('positive fit, R_{eff} = %.4g',R_pos);
negFitLeg = sprintf('negative fit, R_{eff} = %g',R_neg);

leg = legend('data points',totalFitLeg, posFitLeg, negFitLeg, 'Location','northwest');
set(leg,'FontSize',legSize);

hold off;

