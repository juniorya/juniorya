PROGRAM Demo
VAR
    REAL FeedRate;
END_VAR

FeedRate := FeedRate + 1.0;
IF FeedRate > 100.0 THEN
    FeedRate := 0.0;
END_IF;

END_PROGRAM
